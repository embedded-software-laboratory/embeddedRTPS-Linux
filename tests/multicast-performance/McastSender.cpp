/*
 *
 * Author: Clemens Boennen (clemens.boennen@rwth-aachen.de)
 */
#include "McastSender.h"

#include "ucdr/microcdr.h"

#include <iostream>

#define MCASTSENDER_VERBOSE 0
#define PRINT_RESULT_AFTER 1
#define PRINT_SINGLE_SAMPLE_RESULTS 0

using rtps::MulticastTests::MulticastSender;

MulticastSender::MulticastSender(bool useMulticast, bool reliable)
        : m_buffer(SIZE_SAMPLES), m_multicast(useMulticast), m_reliable(reliable){
    prepareRTPS();
    err_t err1 = sys_mutex_new(&m_mutex);
    err_t err2 = sys_sem_new(&m_sem, 0);
    if (err1 != ERR_OK || err2 != ERR_OK) {
        printf("[ ERROR ] Failed to create semaphore/mutex!\n");
    }
}

void MulticastSender::prepareRTPS(){
    
    auto part = m_domain.createParticipant();
    if(part == nullptr){
        printf("[ ERROR ] Failed to create participant\n");
        return;
    }
    m_domain.completeInit();
    if (m_multicast) {
        mp_dataWriter = m_domain.createWriter(*part, "MulticastPerf", "PerformanceType", m_reliable, false);
        mp_dataReader = m_domain.createReader(*part, "MulticastPerfResp", "PerformanceType", m_reliable);
    } else {
        mp_dataWriter = m_domain.createWriter(*part, "MulticastPerf", "PerformanceType", m_reliable, true);
        mp_dataReader = m_domain.createReader(*part, "MulticastPerfResp", "PerformanceType", m_reliable);
    }
    
    if(mp_dataReader == nullptr || mp_dataWriter == nullptr){
        printf("[ ERROR ] Failed to create endpoints.\n");
        return;
    }

    mp_dataReader->registerCallback(responderJumppad, this);
}

void MulticastSender::responderJumppad(void* vp_receiverUnit, const rtps::ReaderCacheChange& cacheChange){
    auto receiverUnit = static_cast<MulticastSender*>(vp_receiverUnit);

    receiverUnit->responderCallback(cacheChange);
}

void MulticastSender::responderCallback(const rtps::ReaderCacheChange& cacheChange){
    #if USE_INTERNAL_CLOCK
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    #endif
    bool success = cacheChange.copyInto(m_buffer.data(), m_buffer.size());

    ucdrBuffer reader;
    ucdr_init_buffer(&reader, &m_buffer[0], m_buffer.size());

    struct MeasurementPacket mp;

    uint8_t scheme[2];
    ucdr_deserialize_array_uint8_t(&reader, scheme, rtps::SMElement::SCHEME_CDR_LE.size());
    uint16_t options;
    ucdr_deserialize_uint16_t(&reader, &options);

    auto success2 = MeasurementPacket_deserialize_topic(&reader, &mp);
    

    if(success && success2){
        #if MCASTSENDER_VERBOSE
        printf("[ INFO  ] Received Message with ID: %d \n", mp.id);
        #endif
        if (mp.p_type == measure::MEA_INIT && mp.id == 0) {
            #if MCASTSENDER_VERBOSE
            printf("[ INFO  ] InitPacket! Partner with Message: %s ID: %d \n", mp.p_textmessage, mp.id);
            #endif
            {
                Lock lock(m_mutex);
                numberOfPartners++;
            }
        } else if ((mp.p_type == measure::MEA_RESPONSE_MULTICAST && m_multicast) ||
                (mp.p_type == measure::MEA_RESPONSE_UNICAST && !m_multicast)) {
            bool finish = false;
            if (mp.id == currentId) {
                Lock lock(m_mutex);
                if (++numberOfResponses == numberOfPartners) {
                    #if USE_INTERNAL_CLOCK
                        std::chrono::duration<double, std::micro> duration(end - m_time);
                        #if PRINT_RESULT_AFTER == 0
                        printf("[RESULT ] ID: %d TIME (microsecs): %f \n", currentId, duration.count());
                        #endif
                        m_rtimes.push_back(duration.count());
                    #else
                        // Toggle PIN
                    #endif
                    finish = true; // this is only to release the lock before signal the semaphore!
                }
            } else {
                #if MCASTSENDER_VEBOSE
                printf("[ WARN  ] Received response with wrong ID: %d. Maybe delayed from prev round. Ignoring.\n", mp.id);
                #endif
            }
            if(finish) {
                sys_sem_signal(&m_sem);
            }
        } else {
            #if MCASTSENDER_VEBOSE
            printf("[ WARN  ] Unexpected Packaet! Ignoring.\n");
            #endif
        }
    }else{
        printf("[ ERROR ] Received message but copying or deserialization failed\n");
    }
}

void MulticastSender::start_time() {
    #if USE_INTERNAL_CLOCK
        m_time = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    #else 
        // TOGGLE PIN OUT
    #endif

}

void MulticastSender::buildAndSendPacket(uint32_t index) {
    currentId = index;
    MeasurementPacket mp;
    if (m_multicast) {
        mp.p_type = measure::MEA_SEND_MULTICAST;
    } else {
        mp.p_type = measure::MEA_SEND_UNICAST;
    }
    strcpy(mp.p_textmessage, getRandomString(STRING_LENGTH, index*2));
    mp.id = index;
    #if USE_INTERNAL_CLOCK
        mp.timestamp = m_time.time_since_epoch().count();
    #else
        mp.timestamp = 0;
    #endif
    
    // Serialize MP
    uint32_t mpsize = MeasurementPacket_size_of_topic(&mp, 0) + 4;
    uint8_t buffer[mpsize];

    ucdrBuffer ubuff;
    ucdr_init_buffer(&ubuff, buffer, mpsize);

    const uint16_t zero_options = 0;
    ucdr_serialize_array_uint8_t(&ubuff, rtps::SMElement::SCHEME_CDR_LE.data(), rtps::SMElement::SCHEME_CDR_LE.size());
    ucdr_serialize_uint16_t(&ubuff, zero_options);

    MeasurementPacket_serialize_topic(&ubuff, &mp);
    auto change = mp_dataWriter->newChange(rtps::ChangeKind_t::ALIVE, buffer, mpsize);
    if(change == nullptr){
        printf("[ ERROR ] History may be full. Sending NOT successful! Abort. \n");
        return;
    }
}

void MulticastSender::sendInitPacket() {
    MeasurementPacket mp;
    mp.p_type = measure::MEA_INIT;
    strcpy(mp.p_textmessage, "InitPhase1");
    mp.id = 0;
    mp.timestamp = 0;

    // Serialize InitMP
    uint32_t mpsize = MeasurementPacket_size_of_topic(&mp, 0) + 4;
    uint8_t buffer[mpsize];

    ucdrBuffer ubuff;
    ucdr_init_buffer(&ubuff, buffer, mpsize);

    const uint16_t zero_options = 0;
    ucdr_serialize_array_uint8_t(&ubuff, rtps::SMElement::SCHEME_CDR_LE.data(), rtps::SMElement::SCHEME_CDR_LE.size());
    ucdr_serialize_uint16_t(&ubuff, zero_options);

    MeasurementPacket_serialize_topic(&ubuff, &mp);
    auto change = mp_dataWriter->newChange(rtps::ChangeKind_t::ALIVE, buffer, mpsize);
    if(change == nullptr){
        printf("[ ERROR ] History may be full. Sending NOT successful! Abort. \n");
        return;
    }
}

void MulticastSender::run(){
    printf("[ INFO  ] Prepairing... Remaining: 15 Secs.\n");
    numberOfPartners = 0;
    numberOfResponses = 0;
    m_rtimes.reserve(NUM_SAMPLES);
    // Wait for other participants to be ready
    printf("[ INFO  ] Waiting for Participants. Remaining: 15 Secs.\n");
    sys_msleep(5000);
    printf("[ INFO  ] Remianing: 10 Secs.\n");
    sys_msleep(5000);

    // Initialization
    sendInitPacket();
    printf("[ INFO  ] Initialization... Getting info about partners. Remaining: 5 Secs.\n");
    sys_msleep(5000);
    printf("[ INFO  ] Initialization done! Found: %d\n", numberOfPartners);

    // Begin measurement
    printf("[ INFO  ] Begin measurement! Number of rounds: %d - Number of Samples per rounds: %d\n", NUM_ROUNDS, NUM_SAMPLES);
    printf("[ INFO  ] Round   mean       min        max        stddev     0.5 q(med) 0.9 q      0.99 q\n");
    for(uint8_t k = 0; k < NUM_ROUNDS; k++) {
        m_rtimes.clear();
        m_rtimes.reserve(NUM_SAMPLES);
        for(uint8_t i = 0; i < NUM_SAMPLES ; i++) {
            {
                Lock lock(m_mutex);
                numberOfResponses = 0;
            }
            start_time();
            buildAndSendPacket(i);
            uint32_t r = sys_arch_sem_wait(&m_sem, 2000);
            #if MCASTSENDER_VERBOSE
            if (r == SYS_ARCH_TIMEOUT) {
                printf("[ WARN  ] Timeout!\n");
            }
            printf("[ INFO  ] Sample ended.\n");
            #endif
        }
        printf("[RESULT ] %-7u ", k+1);
        evaluate();
    }
    
    
    
}

void MulticastSender::evaluate() {
    double num = NUM_SAMPLES;
    double mean = std::accumulate(m_rtimes.begin(), m_rtimes.end(), 0) / num;
    double max = *std::max_element(m_rtimes.begin(), m_rtimes.end());
    double min = *std::min_element(m_rtimes.begin(), m_rtimes.end());

    double var = 0;
    for(auto &d : m_rtimes) {
        var += pow(d - mean,2);

    }

    double stddev = sqrt(var) / m_rtimes.size();

    std::sort(m_rtimes.begin(), m_rtimes.end());

    std::array<double, 4> q = {0.5, 0.9, 0.99};
    std::vector<double> q_result;
    for(auto i : q) {
        auto index = static_cast<size_t>(std::ceil(m_rtimes.size()*i) - 1);
        if(index < m_rtimes.size()) {
            q_result.push_back(m_rtimes[index]);
        } else {
            q_result.push_back(NAN);
        }
    }

    #if PRINT_RESULT_AFTER
    int i = 0;
    #if PRINT_SINGLE_SAMPLE_RESULTS
    for(auto it = m_rtimes.begin(); it != m_rtimes.end(); it++, i++) {
        printf("[RESULT ] ID: %d TIME (microsecs): %f\n", i+1, *it);
    }
    #endif
    #endif
    printf("%-9.2f  %-9.2f  %-9.2f  %-9.2f  %-9.2f  %-9.2f  %-9.2f \n", mean, min, max, stddev, q_result[0], q_result[1], q_result[2]);
}