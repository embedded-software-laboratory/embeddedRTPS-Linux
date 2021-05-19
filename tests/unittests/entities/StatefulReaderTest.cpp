/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rtps/rtps.h"
#include "rtps/discovery/TopicData.h"
#include "rtps/entities/StatefulReader.h"
#include "rtps/messages/MessageTypes.h"
#include "../../mocking/NetworkDriverMock.h"

using namespace rtps;

void setVariableToTrue(void* success, const ReaderCacheChange&){
    *static_cast<bool*>(success) = true; // using callee pointer for feedback
}

static TopicData getDefaultAttributes(){
    const ParticipantId_t arbitraryParticipantId = 1;
    TopicData attributes;
    attributes.topicName[0] = '\0';
    attributes.typeName[0] = '\0';
    attributes.reliabilityKind = ReliabilityKind_t::RELIABLE;
    attributes.endpointGuid.prefix = GUIDPREFIX_UNKNOWN;
    attributes.unicastLocator = getBuiltInUnicastLocator(arbitraryParticipantId);
    return attributes;
}

class StatefulReaderWithoutProxies : public ::testing::Test{
protected:
    NetworkDriverMock mockDriver;
    StatefulReaderT<NetworkDriverMock> reader;

    constexpr static DataSize_t dataSize = 5;
    uint8_t someData[dataSize] = {0};
    SequenceNumber_t nextSN = {0,1};
    Guid_t someGuid = {{1,1,1,1,1,1,1,1,1,1}, {{1,1,1}, EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY}};
    ReaderCacheChange someChange{ChangeKind_t::ALIVE, someGuid, nextSN, someData, dataSize};

    void SetUp() override{
        reader.init(getDefaultAttributes(), mockDriver);
    }
};

TEST_F(StatefulReaderWithoutProxies, newChange_doesntCallCallback){
    bool success = false;
    reader.registerCallback(setVariableToTrue, &success);

    reader.newChange(someChange);

    EXPECT_FALSE(success);
}

class StatefulReaderWithOneProxy : public ::testing::Test{
protected:
    const ParticipantId_t arbitraryParticipantId = 1;
    const Ip4Port_t srcPort = getUserUnicastPort(arbitraryParticipantId);
    NetworkDriverMock mockDriver;
    StatefulReaderT<NetworkDriverMock> reader;

    constexpr static DataSize_t dataSize = 5;
    uint8_t someData[dataSize] = {0};
    SequenceNumber_t nextSN = {0,1};
    Guid_t someGuid = {{1,1,1,1,1,1,1,1,1,1}, {{1,1,1}, EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY}};
    Guid_t anotherGuid = {{2,2,2,2,2,2,2,2,2,2,2,2}, {{2,2,2}, EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY}};
    Locator someLocator = Locator::createUDPv4Locator(1,2,3,4,5);
    ReaderCacheChange expectedCacheChange{ChangeKind_t::ALIVE, someGuid, nextSN, someData, dataSize};
    ReaderCacheChange cacheChangeFromUnknownWriter{ChangeKind_t::ALIVE, anotherGuid, nextSN, someData, dataSize};
    ReaderCacheChange cacheChangeWithDifferentSN{ChangeKind_t::ALIVE, someGuid, ++nextSN, someData, dataSize};

    WriterProxy someProxy{someGuid, someLocator};

    SubmessageHeartbeat hbMsg;

    void SetUp() override{
        reader.init(getDefaultAttributes(), mockDriver);

        someProxy.hbCount = {2};
        reader.addNewMatchedWriter(someProxy);

        hbMsg.header.submessageId = SubmessageKind::HEARTBEAT;
        hbMsg.header.octetsToNextHeader = sizeof(SubmessageHeartbeat) - MessageFactory::numBytesUntilEndOfLength;
        hbMsg.header.flags = FLAG_LITTLE_ENDIAN;
        // Force response by not setting final flag.
        hbMsg.writerId = someProxy.remoteWriterGuid.entityId;
        hbMsg.readerId = reader.m_attributes.endpointGuid.entityId;
        hbMsg.firstSN = SequenceNumber_t{0,1};
        hbMsg.lastSN = SequenceNumber_t{0,50};
        hbMsg.count = {someProxy.hbCount.value + 1};
    }
};

TEST_F(StatefulReaderWithOneProxy, newChange_callsCallbackIfCorrectSN){
    bool success = false;
    reader.registerCallback(setVariableToTrue, &success);

    reader.newChange(expectedCacheChange);

    EXPECT_TRUE(success);
}

TEST_F(StatefulReaderWithOneProxy, newChange_doesntAcceptSameSNTwiceForSameProxy){
    bool success = false;
    reader.registerCallback(setVariableToTrue, &success);

    reader.newChange(expectedCacheChange);
    ASSERT_TRUE(success);
    success = false;

    reader.newChange(expectedCacheChange);
    EXPECT_FALSE(success);
}

TEST_F(StatefulReaderWithOneProxy, newChange_doesntAcceptFromUnknownWriter){
    bool success = false;
    reader.registerCallback(setVariableToTrue, &success);

    reader.newChange(cacheChangeFromUnknownWriter);

    EXPECT_FALSE(success);
}

TEST_F(StatefulReaderWithOneProxy, newChange_doesntAcceptAfterRemovingWriter){
    bool success = false;
    reader.registerCallback(setVariableToTrue, &success);

    reader.removeWriter(someProxy.remoteWriterGuid);
    reader.newChange(expectedCacheChange);

    EXPECT_FALSE(success);
}

TEST_F(StatefulReaderWithOneProxy, newChange_doenstCallsCallbackIfNotRegistered){
    reader.newChange(expectedCacheChange);
    // Expect it doesn't try to call a nullptr
}


TEST_F(StatefulReaderWithOneProxy, onNewHeartBeat_sendsAckNack){
    EXPECT_CALL(mockDriver, sendPacket);

    reader.onNewHeartbeat(hbMsg, someProxy.remoteWriterGuid.prefix);
}

TEST_F(StatefulReaderWithOneProxy, onNewHeartBeat_isNotCalledWithInvalidMsg){
    EXPECT_CALL(mockDriver, sendPacket).Times(0);

    // Count already higher
    hbMsg.count = someProxy.hbCount;
    reader.onNewHeartbeat(hbMsg, someProxy.remoteWriterGuid.prefix);
}

TEST_F(StatefulReaderWithOneProxy, onNewHeartBeat_doesntAcceptSameMsgTwice){
    EXPECT_CALL(mockDriver, sendPacket).Times(1);
    reader.onNewHeartbeat(hbMsg, someProxy.remoteWriterGuid.prefix);
    reader.onNewHeartbeat(hbMsg, someProxy.remoteWriterGuid.prefix);
}

// TODO
TEST_F(StatefulReaderWithOneProxy, DISABLED_onNewHeartBeat_sendsValidAckNack){}
// TODO
TEST_F(StatefulReaderWithOneProxy, DISABLED_onNewHeartBeat_isThreadSafe){}

class StatefulReaderWithTwoProxy : public ::testing::Test{
protected:
    const ParticipantId_t arbitraryParticipantId = 1;
    const Ip4Port_t srcPort = getUserUnicastPort(arbitraryParticipantId);
    NetworkDriverMock mockDriver;
    StatefulReaderT<NetworkDriverMock> reader;

    constexpr static DataSize_t dataSize = 5;
    uint8_t someData[dataSize] = {0};
    SequenceNumber_t nextSN = {0,1};
    Guid_t someGuid = {{1,1,1,1,1,1,1,1,1,1}, {{1,1,1}, EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY}};
    Guid_t anotherGuid = {{2,2,2,2,2,2,2,2,2,2,2,2}, {{2,2,2}, EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY}};
    Locator someLocator = Locator::createUDPv4Locator(1,2,3,4,5);
    ReaderCacheChange firstProxyFirstCacheChange{ChangeKind_t::ALIVE, someGuid, nextSN, someData, dataSize};
    ReaderCacheChange secondProxyFirstCacheChange{ChangeKind_t::ALIVE, anotherGuid, nextSN, someData, dataSize};

    WriterProxy firstProxy{someGuid, someLocator};
    WriterProxy secondProxy{anotherGuid, someLocator};

    void SetUp() override{
        reader.init(getDefaultAttributes(), mockDriver);

        reader.addNewMatchedWriter(firstProxy);
        reader.addNewMatchedWriter(secondProxy);
    }
};

TEST_F(StatefulReaderWithTwoProxy, acceptSameSNForDifferentProxies){
    bool success = false;
    reader.registerCallback(setVariableToTrue, &success);

    reader.newChange(firstProxyFirstCacheChange);
    EXPECT_TRUE(success);
    success = false;
    reader.newChange(secondProxyFirstCacheChange);
    EXPECT_TRUE(success);
}
