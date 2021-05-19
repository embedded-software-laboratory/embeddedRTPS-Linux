/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#ifndef RTPS_READERMOCK_H
#define RTPS_READERMOCK_H

#include <gmock/gmock.h>

#include "rtps/entities/Reader.h"
#include "rtps/storages/HistoryCache.h"

using rtps::SubmessageHeartbeat;
using rtps::GuidPrefix_t;

class ReaderMock : public rtps::Reader{
public:

    ReaderMock() = default;

    explicit ReaderMock(rtps::Guid_t id){
        m_attributes.endpointGuid = id;
    }

    explicit ReaderMock(const rtps::TopicData& topicData){
        m_attributes = topicData;
    }

    ~ReaderMock() override = default;

    MOCK_METHOD1(newChange, void(const rtps::ReaderCacheChange&));
    MOCK_METHOD2(registerCallback, void(rtps::ddsReaderCallback_fp, void*));
    MOCK_METHOD2(onNewHeartbeat, bool(const SubmessageHeartbeat&, const GuidPrefix_t&));
    MOCK_METHOD1(addNewMatchedWriter, bool(const rtps::WriterProxy&));
    MOCK_METHOD1(removeWriter, void(const rtps::Guid_t&));
    MOCK_METHOD1(removeWriterOfParticipant, void(const rtps::GuidPrefix_t&));
};

#endif //RTPS_READERMOCK_H
