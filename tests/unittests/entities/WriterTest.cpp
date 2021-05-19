/*
 * This test suites contain common tests for reliable and best-effort writers.
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */


#include <gtest/gtest.h>

#include "rtps/entities/StatelessWriter.h"
#include "rtps/rtps.h"

#include "../../CustomMatcher.h"
#include "../../globals.h"
#include "../../mocking/NetworkDriverMock.h"

using namespace rtps;
using ::testing::ByRef;
using ::testing::_;
using ::testing::AtLeast;

using WriterTypes = ::testing::Types<StatelessWriterT<NetworkDriverMock>, StatefulWriterT<NetworkDriverMock>>;


template <typename T>
class FreshlyCreatedWriter : public ::testing::Test{
protected:
    const TopicKind_t arbitraryType = TopicKind_t::NO_KEY;
    const ParticipantId_t someParticipantId = 1;
    const TopicData attributes{TestGlobals::someWriterGuid,
                               ReliabilityKind_t::BEST_EFFORT,
                               getUserUnicastLocator(someParticipantId)};

    NetworkDriverMock transport;
    T writer;
    std::vector<uint8_t> data = {0, 1, 2, 3, 4};

    void SetUp() override{
        writer.init(attributes, arbitraryType, nullptr, transport);
    }
};


TYPED_TEST_SUITE(FreshlyCreatedWriter, WriterTypes);

TYPED_TEST(FreshlyCreatedWriter, newChange_IncreasesSequenceNumber){
    TypeParam& writer = this->writer;
    auto lastSN = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0)->sequenceNumber;

    auto* secondChange = writer.newChange(ChangeKind_t::ALIVE, nullptr, 0);
    ASSERT_NE(secondChange, nullptr);

    EXPECT_EQ(secondChange->sequenceNumber, ++lastSN);
}

TYPED_TEST(FreshlyCreatedWriter, newChange_SetsCorrectValues){
    TypeParam& writer = this->writer;
    ChangeKind_t expectedKind = ChangeKind_t::ALIVE;

    const CacheChange* change = writer.newChange(expectedKind, this->data.data(), this->data.size());

    ASSERT_NE(change, nullptr);
    EXPECT_EQ(change->kind, expectedKind);
    EXPECT_EQ(change->data.firstElement->tot_len, this->data.size());
    EXPECT_THAT(change->data, PBufContains(this->data));
}

TYPED_TEST(FreshlyCreatedWriter, progress_doesNotSendAnyChangesAfterAddingOne){
    TypeParam& writer = this->writer;

    const CacheChange* change = writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());
    ASSERT_NE(change, nullptr);
    EXPECT_CALL(this->transport, sendPacket).Times(0); // No remote reader

    writer.progress();
}

template <typename T>
class WriterWithProxyAndFullHistory : public ::testing::Test{
protected:
    const ParticipantId_t someParticipantId = 1;
    const TopicData writerAttributes{TestGlobals::someWriterGuid,
                                     ReliabilityKind_t::BEST_EFFORT,
                                     getUserUnicastLocator(someParticipantId)};

    NetworkDriverMock transport;
    T writer;
    const std::vector<uint8_t> data = {0, 1, 2, 3, 4};
    std::vector<SequenceNumber_t> sns;

    void SetUp() override{
        writer.init(writerAttributes, TopicKind_t::WITH_KEY, nullptr, transport);
        writer.addNewMatchedReader(ReaderProxy(TestGlobals::someReaderGuid, TestGlobals::someUserUnicastLocator));
        for(int i=0; i < Config::HISTORY_SIZE; i++){
            const CacheChange* change = writer.newChange(ChangeKind_t::ALIVE, data.data(), data.size());
            sns.push_back(change->sequenceNumber);
            ASSERT_NE(change, nullptr);
        }
    };
};


TYPED_TEST_SUITE(WriterWithProxyAndFullHistory, WriterTypes);

TYPED_TEST(WriterWithProxyAndFullHistory, progess_sendsMessageAfterAddingOneMore){
    TypeParam& writer = this->writer;
    const CacheChange* change = writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());
    ASSERT_NE(change, nullptr);

    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); // Fallback for heartbeat etc.
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(ByRef(this->sns[1])))); // skipped first

    writer.progress();
}

TYPED_TEST(WriterWithProxyAndFullHistory, progess_sendsAllMessagesAfterSendingOneAndAddingANewOne){
    TypeParam& writer = this->writer;
    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); // Fallback for heartbeat etc.
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(ByRef(this->sns[0])))).Times(1);
    writer.progress();
    testing::Mock::VerifyAndClearExpectations(&writer); // This and EXPECT_CALL are used to avoid warning about uninteresting call

    const CacheChange* change = writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());
    this->sns.push_back(change->sequenceNumber);
    ASSERT_NE(change, nullptr);

    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); // Fallback for heartbeat etc.
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);

    for(auto it = this->sns.begin() + 1; it != this->sns.end(); ++it){
        EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(ByRef(*it)))).Times(1);
    }
    const int randomValueToCallMoreOftenThanNecessary = 5;
    for(int i=0; i < Config::HISTORY_SIZE + randomValueToCallMoreOftenThanNecessary; i++){
        writer.progress();
    }
    printf("reached end");
}

template <typename T>
class WriterWithTwoProxies : public ::testing::Test{
protected:
    const ParticipantId_t someParticipantId = 1;
    const TopicData writerAttributes{TestGlobals::someWriterGuid,
                                     ReliabilityKind_t::BEST_EFFORT,
                                     getUserUnicastLocator(someParticipantId)};

    NetworkDriverMock transport;
    T writer;
    const std::vector<uint8_t> data = {0, 1, 2, 3, 4};
    std::vector<SequenceNumber_t> sns;

    void SetUp() override { 
        writer.init(writerAttributes, TopicKind_t::WITH_KEY, nullptr, this->transport);
        writer.addNewMatchedReader(TestGlobals::someReaderProxy);
        writer.addNewMatchedReader(TestGlobals::someReaderProxy2);
    };
};


TYPED_TEST_SUITE(WriterWithTwoProxies, WriterTypes);

TYPED_TEST(WriterWithTwoProxies, progess_sendsMessageToBothProxies){
    TypeParam& writer = this->writer;
    auto* change = writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());

    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); // Fallback for heartbeat etc.
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);

    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(ByRef(change->sequenceNumber)))).Times(2);

    writer.progress();
}

template <typename T>
class WriterWithMulticastProxies : public ::testing::Test{
protected:
    const ParticipantId_t someParticipantId = 1;
    const TopicData writerAttributes{TestGlobals::someWriterGuid,
                                     ReliabilityKind_t::BEST_EFFORT,
                                     getUserUnicastLocator(someParticipantId)};

    NetworkDriverMock transport;
    T writer;
    T writer_unicast;
    const std::vector<uint8_t> data = {0, 1, 2, 3, 4};
    std::vector<SequenceNumber_t> sns;

    void SetUp() override{
        writer.init(writerAttributes, TopicKind_t::WITH_KEY, nullptr, this->transport);
        writer.addNewMatchedReader(TestGlobals::someMulticastReaderProxy);
        writer.addNewMatchedReader(TestGlobals::someMulticastReaderProxy2);
        writer_unicast.init(writerAttributes, TopicKind_t::WITH_KEY, nullptr, this->transport, true);
        writer_unicast.addNewMatchedReader(TestGlobals::someMulticastReaderProxy);
        writer_unicast.addNewMatchedReader(TestGlobals::someMulticastReaderProxy2);
    };
};

TYPED_TEST_SUITE(WriterWithMulticastProxies, WriterTypes);

TYPED_TEST(WriterWithMulticastProxies, newChange_sendMulticast) {
    TypeParam& writer = this->writer;

    // Two Readers, send only once!
    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); //Hearbeat
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(SequenceNumber_t{0,1}))).Times(1);

    writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());

    // Simulating Thread
    writer.progress();
}

TYPED_TEST(WriterWithMulticastProxies, newChange_sendMulticastEnforceUnicast) {  
    TypeParam& writer = this->writer_unicast;

    // Two Readers, send twice!
    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); //Hearbeat
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);
    EXPECT_CALL(this->transport, sendPacket(HasDataWithSequenceNumber(SequenceNumber_t{0,1}))).Times(2);

    writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());

    // Simulating Thread
    writer.progress();
}

TYPED_TEST(WriterWithMulticastProxies, removeByPartId_checkIfRemoved) {  
    TypeParam& writer = this->writer;

    writer.removeReaderOfParticipant(TestGlobals::someGuidPrefix);

    // Two Readers, send twice!
    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); //Hearbeat
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);
    EXPECT_CALL(this->transport, sendPacket(HasDataWithIP(TestGlobals::someUserUnicastLocator2.getIp4Address()))).Times(1);

    writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());

    // Simulating Thread
    writer.progress();
}

TYPED_TEST(WriterWithMulticastProxies, removeByPartId_checkIfRemovedUnicast) {  
    TypeParam& writer = this->writer_unicast;

    writer.removeReaderOfParticipant(TestGlobals::someGuidPrefix);

    // Two Readers, send twice!
    EXPECT_CALL(this->transport, sendPacket(_)).Times(AtLeast(0)); //Hearbeat
    EXPECT_CALL(this->transport, sendPacket(HasDataSubmessage())).Times(0);
    EXPECT_CALL(this->transport, sendPacket(HasDataWithIP(TestGlobals::someUserUnicastLocator2.getIp4Address()))).Times(1);

    writer.newChange(ChangeKind_t::ALIVE, this->data.data(), this->data.size());

    // Simulating Thread
    writer.progress();
}

