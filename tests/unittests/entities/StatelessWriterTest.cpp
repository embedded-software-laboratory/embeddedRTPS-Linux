/*
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include <gtest/gtest.h>

#include "rtps/entities/StatelessWriter.h"
#include "rtps/rtps.h"

#include "../../CustomMatcher.h"
#include "../../globals.h"
#include "../../mocking/NetworkDriverMock.h"

using namespace rtps;

/**
 * Some tests, which are common with other types of writers, are outsourced to WriterTest.cpp
 */


class EmptyStatelessWriterWithoutKey : public ::testing::Test{
protected:
    const ParticipantId_t someParticipantId = 1;
    const TopicData attributes{TestGlobals::someWriterGuid,
                                      ReliabilityKind_t::BEST_EFFORT,
                                      getUserUnicastLocator(someParticipantId)};

    NetworkDriverMock transport;
    StatelessWriterT<NetworkDriverMock> writer;
    static const DataSize_t size = 5;
    const uint8_t data[size] = {0, 1, 2, 3, 4};

    void SetUp() override{
        writer.init(attributes, TopicKind_t::NO_KEY, nullptr, transport);
    }
};

TEST_F(EmptyStatelessWriterWithoutKey, newChange_IgnoresAllKindsThatAreNotAlive){
    ChangeKind_t irrelevantKinds[] = {ChangeKind_t::INVALID,
                           ChangeKind_t::NOT_ALIVE_DISPOSED,
                           ChangeKind_t::NOT_ALIVE_UNREGISTERED};

    SequenceNumber_t oldSN = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0)->sequenceNumber;
    for(auto kind : irrelevantKinds){
        const CacheChange* change = writer.newChange(kind, nullptr, 0);
        EXPECT_EQ(change, nullptr);
        SequenceNumber_t newSN = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0)->sequenceNumber;
        EXPECT_EQ(newSN, ++oldSN);
    }
}

class EmptyStatelessWriterWithKey : public ::testing::Test{
protected:
    const ParticipantId_t someParticipantId = 1;
    const TopicData attributes{TestGlobals::someWriterGuid,
                                      ReliabilityKind_t::BEST_EFFORT,
                                      getUserUnicastLocator(someParticipantId)};

    NetworkDriverMock transport;
    StatelessWriterT<NetworkDriverMock> writer;
    void SetUp() override{
        writer.init(attributes, TopicKind_t::WITH_KEY, nullptr, transport);
    };
};

TEST_F(EmptyStatelessWriterWithKey, newChange_IgnoresKindInvalid){
    SequenceNumber_t lastSN = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0)->sequenceNumber;
    const CacheChange* change = writer.newChange(ChangeKind_t::INVALID, nullptr, 0);

    SequenceNumber_t newSN = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0)->sequenceNumber;
    EXPECT_EQ(change, nullptr);
    EXPECT_EQ(newSN, ++lastSN);

}

// Disabled as we only only allow ALIVE
TEST_F(EmptyStatelessWriterWithKey, DISABLED_newChange_AddsAllKindsBesideInvalid){
    ChangeKind_t relevantKinds[] = {ChangeKind_t::ALIVE,
                                    ChangeKind_t::NOT_ALIVE_DISPOSED,
                                    ChangeKind_t::NOT_ALIVE_UNREGISTERED};

    SequenceNumber_t lastSN = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0)->sequenceNumber;

    for(auto kind : relevantKinds){
        const CacheChange* change = writer.newChange(kind, nullptr, 0);
        EXPECT_EQ(change->kind, kind);
        EXPECT_EQ(change->sequenceNumber, ++lastSN);
    }
}


