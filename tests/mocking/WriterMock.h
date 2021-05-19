/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#ifndef RTPS_WRITERMOCK_H
#define RTPS_WRITERMOCK_H

#include <gmock/gmock.h>

#include "rtps/entities/Writer.h"
#include "rtps/storages/HistoryCache.h"

class WriterMock : public rtps::Writer{
public:

    WriterMock() = default;

    explicit WriterMock(const rtps::TopicData& topicData){
        m_attributes = topicData;
    }

    MOCK_METHOD1(addNewMatchedReader, bool(const rtps::ReaderProxy&));
    MOCK_METHOD1(removeReader, void(const rtps::Guid_t&));
    MOCK_METHOD1(removeReaderOfParticipant, void(const rtps::GuidPrefix_t&));
    MOCK_METHOD0(progress, void());
    MOCK_METHOD3(newChange, const rtps::CacheChange*(rtps::ChangeKind_t, const uint8_t*, rtps::DataSize_t));
    MOCK_METHOD0(setAllChangesToUnsent, void());
    MOCK_METHOD2(onNewAckNack, void(const rtps::SubmessageAckNack&, const rtps::GuidPrefix_t&));
};
#endif //RTPS_WRITERMOCK_H
