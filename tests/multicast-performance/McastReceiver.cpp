/*
 *
 * Author: Clemens Boennen (clemens.boennen@rwth-aachen.de)
 */
#include "McastReceiver.h"

#include "ucdr/microcdr.h"

#include <iostream>

#define MCASTRECEIVER_VERBOSE 0

using rtps::MulticastTests::MulticastReceiver;

MulticastReceiver::MulticastReceiver(bool useMulticast, bool reliable)
        : m_buffer(SIZE_SAMPLES), m_multicast(useMulticast), m_reliable(reliable){
    prepareRTPS();
}

void MulticastReceiver::prepareRTPS(){
    
    auto part = m_domain.createParticipant();
    if(part == nullptr){
        printf("[ ERROR ] Failed to create participant\n");
        return;
    }
    m_domain.completeInit();
    if (m_multicast) {
        mp_dataReader = m_domain.createReader(*part, "MulticastPerf", "PerformanceType", m_reliable, transformIP4ToU32(239,255,0,2));
        mp_dataWriter = m_domain.createWriter(*part, "MulticastPerfResp", "PerformanceType", m_reliable, true);
    } else {
        mp_dataReader = m_domain.createReader(*part, "MulticastPerf", "PerformanceType", m_reliable);
        mp_dataWriter = m_domain.createWriter(*part, "MulticastPerfResp", "PerformanceType", m_reliable, true);
    }
    
    if(mp_dataReader == nullptr || mp_dataWriter == nullptr){
        printf("[ ERROR ] Failed to create endpoints.\n");
        return;
    }

    mp_dataReader->registerCallback(responderJumppad, this);
}

void MulticastReceiver::responderJumppad(void* vp_receiverUnit, const rtps::ReaderCacheChange& cacheChange){
    auto receiverUnit = static_cast<MulticastReceiver*>(vp_receiverUnit);

    receiverUnit->responderCallback(cacheChange);
}

void MulticastReceiver::responderCallback(const rtps::ReaderCacheChange& cacheChange){

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
        #if MCASTRECEIVER_VERBOSE
        printf("[ INFO  ] Received Message with ID: %d Sending Response!\n", mp.id);
        #endif
        sendResponse(mp);
    }else{
        printf("[ ERROR ] Received message but copying or deserialization failed\n");
    }
}

void MulticastReceiver::sendResponse(MeasurementPacket mp) {
    if(mp.p_type != measure::MEA_INIT) {
        if(m_multicast) {
            mp.p_type = measure::MEA_RESPONSE_MULTICAST;
        } else if(!m_multicast) {
            mp.p_type = measure::MEA_RESPONSE_UNICAST;
        }
    }

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

void MulticastReceiver::run(){
    while(true); // Just serve all the time
}
