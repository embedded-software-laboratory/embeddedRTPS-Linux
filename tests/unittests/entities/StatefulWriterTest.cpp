/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */


#include "gtest/gtest.h"
#include "../../mocking/NetworkDriverMock.h"
#include "../../CustomMatcher.h"
#include "../../globals.h"

#include "rtps/rtps.h"

/*
 * Here we should test things like the push mode and responses to acknacks.
 * Unfortunately, the current interface does not support an easy testing
 * without deserializing the packet again.
 */
using namespace rtps;

using ::testing::AtLeast;
using ::testing::ByRef;
using ::testing::_;

class AStatefulWriter : public ::testing::Test{
protected:
    const TopicKind_t arbitraryType = TopicKind_t::NO_KEY;
    const ParticipantId_t someParticipantId = 1;
    const TopicData attributes{TestGlobals::someWriterGuid,
                               ReliabilityKind_t::BEST_EFFORT,
                               getUserUnicastLocator(someParticipantId)};
    NetworkDriverMock transport;
    StatefulWriterT<NetworkDriverMock> writer;
    const std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};

    void SetUp() override{
        writer.init(attributes, arbitraryType, nullptr, transport);
    }
};

TEST_F(AStatefulWriter, onNewAckNack_sendsEachMissingOnce){
    SequenceNumber_t sn{0,1};
    const rtps::CacheChange* lastChangeAdded = nullptr;
    for(auto i=0; i<5; ++i){
        lastChangeAdded = writer.newChange(ChangeKind_t::ALIVE, data.data(), data.size());
        ASSERT_NE(lastChangeAdded, nullptr);
        ASSERT_EQ(lastChangeAdded->sequenceNumber, sn++);
    }
    writer.addNewMatchedReader(TestGlobals::someReaderProxy);

    SubmessageAckNack acknack;
    acknack.header = {}; // Shouldn't matter
    acknack.count = Count_t{1};
    acknack.readerId = TestGlobals::someReaderId;
    acknack.writerId = attributes.endpointGuid.entityId;
    acknack.readerSNState = SequenceNumberSet(SequenceNumber_t{0,2}); // receive first already
    acknack.readerSNState.numBits = 4;
    acknack.readerSNState.bitMap[0] = uint32_t{1} << (31-0) | uint32_t{1} << (31-1) |
                                      uint32_t{1} << (31-2) | uint32_t{1} << (31-3);

    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); // Fallback for heartbeat etc.
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);

    for(auto exp_sn = acknack.readerSNState.base; exp_sn <= lastChangeAdded->sequenceNumber; ++exp_sn){
        EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(exp_sn))).Times(1);
    }

    writer.onNewAckNack(acknack, TestGlobals::someGuidPrefix); // Guid Prefix matters!
}

TEST_F(AStatefulWriter, onNewAckNack_sendsMissingPacketsAndAllBehindDefinedRange){
    SequenceNumber_t sn{0,1};
    const rtps::CacheChange* lastChangeAdded = nullptr;
    for(auto i=0; i<5; ++i){
        lastChangeAdded = writer.newChange(ChangeKind_t::ALIVE, data.data(), data.size());
        ASSERT_NE(lastChangeAdded, nullptr);
        ASSERT_EQ(lastChangeAdded->sequenceNumber, sn++);
    }
    writer.addNewMatchedReader(TestGlobals::someReaderProxy);

    SubmessageAckNack acknack;
    acknack.header = {}; // Shouldn't matter
    acknack.count = Count_t{1};
    acknack.readerId = TestGlobals::someReaderId;
    acknack.writerId = attributes.endpointGuid.entityId;
    acknack.readerSNState = SequenceNumberSet(SequenceNumber_t{0,2}); // receive first already
    acknack.readerSNState.numBits = 3;
    acknack.readerSNState.bitMap[0] = uint32_t{1} << (31-0) | uint32_t{1} << (31-2);

    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); // Fallback for heartbeat etc.
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);

    // Missing -> send
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(SequenceNumber_t{0,2}))).Times(1);
    // Not missing -> don't send
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(SequenceNumber_t{0,3}))).Times(0);
    // Missing -> send
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(SequenceNumber_t{0,4}))).Times(1);
    // Not within range -> send
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(SequenceNumber_t{0,5}))).Times(1);

    writer.onNewAckNack(acknack, TestGlobals::someGuidPrefix); // Guid Prefix matters!
}