/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de), Clemens Boennen (clemens.boennen@rwth-aachen.de)
 */
#include "Receiver.h"

#include "HelloWorld.h"

#include "ucdr/microcdr.h"

#include <iostream>

using rtps::tests::Receiver;

Receiver::Receiver(uint32_t maxMsgSizeInBytes)
        : m_buffer(maxMsgSizeInBytes){
    prepareRTPS();
}

void Receiver::prepareRTPS(){
    
    auto part = m_domain.createParticipant();
    if(part == nullptr){
        std::cout << "Failed to create participant\n";
        return;
    }
    m_domain.completeInit();
    mp_dataReader = m_domain.createReader(*part, "HelloWorldTopic", "HelloWorld", false, transformIP4ToU32(239,255,0,2));
    
    if(mp_dataReader == nullptr){
        std::cout << "Failed to create endpoints.\n";
        std::terminate();
        return;
    }

    mp_dataReader->registerCallback(responderJumppad, this);
}

void Receiver::responderJumppad(void* vp_receiverUnit, const rtps::ReaderCacheChange& cacheChange){
    auto receiverUnit = static_cast<Receiver*>(vp_receiverUnit);

    receiverUnit->responderCallback(cacheChange);
}

void Receiver::responderCallback(const rtps::ReaderCacheChange& cacheChange){

    bool success = cacheChange.copyInto(m_buffer.data(), m_buffer.size());

    ucdrBuffer reader;
    ucdr_init_buffer(&reader, &m_buffer[0], m_buffer.size());

    struct HelloWorld hw;

    uint8_t scheme[2];
    ucdr_deserialize_array_uint8_t(&reader, scheme, rtps::SMElement::SCHEME_CDR_LE.size());
    uint16_t options;
    ucdr_deserialize_uint16_t(&reader, &options);

    auto success2 = HelloWorld_deserialize_topic(&reader, &hw);

    if(success && success2){
        printf("Received Message %s with index %d\n", hw.message, hw.index);
    }else{
        printf("Received hello world message but copying or deserialization failed\n");
    }
}

void Receiver::run(){
    while(true){
      m_domain.printInfo();
      sleep(1);
    }
}
