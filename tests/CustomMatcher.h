/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#ifndef RTPS_CUSTOMMATCHER_H
#define RTPS_CUSTOMMATCHER_H

#include <gmock/gmock.h>
#include "rtps/storages/PBufWrapper.h"
#include "rtps/communication/PacketInfo.h"
#include "rtps/messages/MessageTypes.h"
#include "lwip/pbuf.h"


MATCHER_P(PBufContains, data, ""){
    pbuf* current = arg.firstElement;

    return current != nullptr && (pbuf_memcmp(current, 0, data.data(), data.size()) == 0);

}

MATCHER_P(HasDataWithIP, expected_ip, ""){
    rtps::PacketInfo& packetInfo = arg;
    return packetInfo.destAddr.addr == expected_ip.addr;
}

MATCHER_P(HasDataWithSequenceNumber, expected_sn, ""){
    rtps::PacketInfo& packetInfo = arg;
    auto posOfCurrentSubMessage = rtps::Header::getRawSize();
    std::array<uint8_t, rtps::SubmessageHeader::getRawSize()> serializedSubMsg;
    rtps::SubmessageHeader subHeader;

    while(posOfCurrentSubMessage < packetInfo.buffer.firstElement->tot_len){
        auto err = pbuf_copy_partial(packetInfo.buffer.firstElement, serializedSubMsg.data(),
                                     serializedSubMsg.size(), posOfCurrentSubMessage);
        if(err != serializedSubMsg.size()){
            return false;
        }

        rtps::MessageProcessingInfo msgInfo(serializedSubMsg.data(), serializedSubMsg.size());
        rtps::deserializeMessage(msgInfo, subHeader);
        if(subHeader.submessageId == rtps::SubmessageKind::DATA){
            std::vector<uint8_t> serializedDataSubMsg(rtps::SubmessageHeader::getRawSize() + subHeader.octetsToNextHeader);
            rtps::SubmessageData dataSubMsg;
            err = pbuf_copy_partial(packetInfo.buffer.firstElement, serializedDataSubMsg.data(),
                                    serializedDataSubMsg.size(), posOfCurrentSubMessage);
            if(err != serializedDataSubMsg.size()){
                return false;
            }

            rtps::MessageProcessingInfo dataMsgInfo(serializedDataSubMsg.data(), serializedDataSubMsg.size());
            rtps::deserializeMessage(dataMsgInfo, dataSubMsg);
            if(dataSubMsg.writerSN == expected_sn){
                return true;
            }else{
                *result_listener << "Found writerSN: (" << dataSubMsg.writerSN.high << "," << dataSubMsg.writerSN.low << ")";
            }
        }

        posOfCurrentSubMessage += subHeader.octetsToNextHeader + rtps::SubmessageHeader::getRawSize();
    }

    return false;
}

MATCHER(HasDataSubmessage, ""){
    bool result = false;
    rtps::PacketInfo& packetInfo = arg;
    auto posOfCurrentSubMessage = rtps::Header::getRawSize();
    std::array<uint8_t, rtps::SubmessageHeader::getRawSize()> serializedSubMsg;
    rtps::SubmessageHeader subHeader;

    while(posOfCurrentSubMessage < packetInfo.buffer.firstElement->tot_len){
        auto err = pbuf_copy_partial(packetInfo.buffer.firstElement, serializedSubMsg.data(),
                                     serializedSubMsg.size(), posOfCurrentSubMessage);
        if(err != serializedSubMsg.size()){
            return false;
        }

        rtps::MessageProcessingInfo msgInfo(serializedSubMsg.data(), serializedSubMsg.size());
        rtps::deserializeMessage(msgInfo, subHeader);
        if(subHeader.submessageId == rtps::SubmessageKind::DATA){
            result = true;

            std::vector<uint8_t> serializedDataSubMsg(rtps::SubmessageHeader::getRawSize() + subHeader.octetsToNextHeader);
            rtps::SubmessageData dataSubMsg;
            err = pbuf_copy_partial(packetInfo.buffer.firstElement, serializedDataSubMsg.data(),
                                    serializedDataSubMsg.size(), posOfCurrentSubMessage);
            if(err != serializedDataSubMsg.size()){
                return false;
            }

            rtps::MessageProcessingInfo dataMsgInfo(serializedDataSubMsg.data(), serializedDataSubMsg.size());
            rtps::deserializeMessage(dataMsgInfo, dataSubMsg);
            std::cout << "Found writerSN: (" << dataSubMsg.writerSN.high << "," << dataSubMsg.writerSN.low << ")\n";
        }

        posOfCurrentSubMessage += subHeader.octetsToNextHeader + rtps::SubmessageHeader::getRawSize();
    }

    return result;
}

#endif //RTPS_CUSTOMMATCHER_H
