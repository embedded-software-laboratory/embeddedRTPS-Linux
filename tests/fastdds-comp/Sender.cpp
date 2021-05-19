/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de), Clemens Boennen (clemens.boennen@rwth-aachen.de)
 *
 */

#include "Sender.h"

#include "rtps/rtps.h"
#include "rtps/entities/Domain.h"
#include "HelloWorld.h"

#include "ucdr/microcdr.h"
#include "rtps/messages/MessageTypes.h"

#include <iostream>
#include <numeric>
#include <cmath>
#include <algorithm>


using rtps::tests::Sender;

Sender::Sender(uint32_t numSamples)
    : m_numSamples(numSamples), m_index(1){
    prepareRTPS();
}

void Sender::prepareRTPS(){
    
    auto part = m_domain.createParticipant();
    if(part == nullptr){
       std::cout << "Failed to create participant\n";
        return;
    }
    m_domain.completeInit();
    mp_dataWriter = m_domain.createWriter(*part, "HelloWorldTopic", "HelloWorld", true);

    if(mp_dataWriter == nullptr){
        std::cout << "Failed to create endpoints.\n";
        return;
    }

}

void Sender::run() {
    std::cout << "Waiting 15 sec for startup...." << '\n';
    sys_msleep(5000); // Wait for initialization
    std::cout << "Go!" << '\n';

    printf("Sending HelloWorldPackets: \n", m_numSamples);

    for (uint32_t i = 0; i < m_numSamples; i++) {
        runWithIndex(m_index);
        printf("Hello WorldPacket sent: Index %d \n", m_index);
        m_index++;
        sys_msleep(100);
    }
}

void Sender::runWithIndex(uint32_t index){
    struct HelloWorld hw;

    hw.index = index;
    strcpy(hw.message, "HelloWorld");

    uint32_t tsize = HelloWorld_size_of_topic(&hw, 0) + 4;

    uint8_t buffer[tsize];

    ucdrBuffer writer;
    ucdr_init_buffer(&writer, buffer, tsize);

    const uint16_t zero_options = 0;
    ucdr_serialize_array_uint8_t(&writer, rtps::SMElement::SCHEME_CDR_LE.data(), rtps::SMElement::SCHEME_CDR_LE.size());
    ucdr_serialize_uint16_t(&writer, zero_options);

    HelloWorld_serialize_topic(&writer, &hw);
    auto change = mp_dataWriter->newChange(rtps::ChangeKind_t::ALIVE, buffer, tsize);
    if(change == nullptr){
        std::cout << "History full. Abort. \n";
        return;
    }

}

