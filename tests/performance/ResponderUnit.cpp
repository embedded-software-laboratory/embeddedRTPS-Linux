/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */
#include "ResponderUnit.h"

#include "LatencyTopic.h"

#include <iostream>

using rtps::tests::ResponderUnit;

ResponderUnit::ResponderUnit(uint32_t maxMsgSizeInBytes)
        : m_buffer(maxMsgSizeInBytes){
    prepareRTPS();
}

void ResponderUnit::prepareRTPS(){
    
    auto part = m_domain.createParticipant();
    if(part == nullptr){
        std::cout << "Failed to create participant\n";
        return;
    }
    m_domain.completeInit();
    mp_dataWriter = m_domain.createWriter(*part, "LatencyTest_SUB2PUB", "LatencyType", false);
    mp_dataReader = m_domain.createReader(*part, "LatencyTest_PUB2SUB", "LatencyType", false);
    
    if(mp_dataWriter == nullptr || mp_dataReader == nullptr){
        std::cout << "Failed to create endpoints.\n";
        return;
    }

    mp_dataReader->registerCallback(responderJumppad, this);
}

void ResponderUnit::responderJumppad(void* vp_responderUnit, const rtps::ReaderCacheChange& cacheChange){
    auto responderUnit = static_cast<ResponderUnit*>(vp_responderUnit);

    responderUnit->responderCallback(cacheChange);
}

void ResponderUnit::responderCallback(const rtps::ReaderCacheChange& cacheChange){

    bool success = cacheChange.copyInto(m_buffer.data(), m_buffer.size());
    if(success){
        mp_dataWriter->newChange(rtps::ChangeKind_t::ALIVE, m_buffer.data(), cacheChange.size);
    }else{
        printf("Received hello world message but copying failed\n");
    }
}

void ResponderUnit::run(){
    while(true); // Just serve all the time
}
