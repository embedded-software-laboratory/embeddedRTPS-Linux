/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rtps/discovery/SEDPAgent.h"
#include "rtps/discovery/ParticipantProxyData.h"
#include "rtps/entities/Participant.h"
#include "../../mocking/WriterMock.h"
#include "../../mocking/ReaderMock.h"

using rtps::SEDPAgent;
using rtps::Participant;
using testing::_;
using testing::NiceMock;

struct TestSEDPAgent : public SEDPAgent{
    void onNewPublisher(const rtps::TopicData& data){
        SEDPAgent::onNewPublisher(data);
    }

    void onNewSubscriber(const rtps::TopicData& data){
        SEDPAgent::onNewSubscriber(data);
    }
};
class AnSEDPAgent : public ::testing::Test{
protected:
    rtps::GuidPrefix_t somePrefix = {1};
    rtps::GuidPrefix_t someRemotePrefix = {2};
    Participant someParticipant{somePrefix, 0};
    rtps::ParticipantProxyData someRemoteParticipant{{someRemotePrefix,0}}; // needs to have a remote participant with matching GUID Prefix!!
    WriterMock spdpWriter;
    NiceMock<ReaderMock> spdpReader;
    NiceMock<WriterMock> sedpPubWriter;
    NiceMock<ReaderMock> sedpPubReader;
    NiceMock<WriterMock> sedpSubWriter;
    NiceMock<ReaderMock> sedpSubReader;
    rtps::BuiltInEndpoints endpoints;
    TestSEDPAgent agent;

    rtps::Locator someLocator = rtps::Locator::createUDPv4Locator(192,168,0,42, 4710);
    rtps::EntityId_t bestEffortReaderEId{{1,2,4}, rtps::EntityKind_t::USER_DEFINED_READER_WITHOUT_KEY};
    rtps::EntityId_t bestEffortWriterEId{{1,2,6}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    rtps::TopicData bestEffortReaderData{{someRemotePrefix,bestEffortReaderEId}, rtps::ReliabilityKind_t::BEST_EFFORT, someLocator};
    rtps::TopicData bestEffortWriterData{{someRemotePrefix,bestEffortWriterEId}, rtps::ReliabilityKind_t::BEST_EFFORT, someLocator};

    void SetUp() override{
        endpoints.spdpWriter = &spdpWriter;
        endpoints.spdpReader = &spdpReader;
        endpoints.sedpPubWriter = &sedpPubWriter;
        endpoints.sedpPubReader = &sedpPubReader;
        endpoints.sedpSubWriter = &sedpSubWriter;
        endpoints.sedpSubReader = &sedpSubReader;
        someParticipant.addNewRemoteParticipant(someRemoteParticipant); // remote Participant
        agent.init(someParticipant, endpoints); // Shall not crash because only spdp's are required
    }

};

TEST_F(AnSEDPAgent, init_onlyRequiresSPDPEndpoints){
    SEDPAgent agent;
    endpoints.sedpPubWriter = nullptr;
    endpoints.sedpPubReader = nullptr;
    endpoints.sedpSubWriter = nullptr;
    endpoints.sedpSubReader = nullptr;

    agent.init(someParticipant, endpoints);
}

TEST_F(AnSEDPAgent, addReader_IgnoredIfNoSubWriterAdded){
    SEDPAgent agent;
    endpoints.sedpSubWriter = nullptr;
    agent.init(someParticipant, endpoints);
    ReaderMock reader(bestEffortReaderData);

    agent.addReader(reader);
}

TEST_F(AnSEDPAgent, addWriter_IgnoredIfNoPubWriterAdded){
    SEDPAgent agent;
    endpoints.sedpPubWriter = nullptr;
    agent.init(someParticipant, endpoints);
    WriterMock writer(bestEffortWriterData);

    agent.addWriter(writer);
}

class CallbackClass{
public:
    MOCK_METHOD0(callback, void());
    static void jumppad(void* arg){
        static_cast<CallbackClass*>(arg)->callback();
    }
};

TEST_F(AnSEDPAgent, doesntCallPublisherCallbackIfNothingMatched){
    CallbackClass cbObject;

    agent.registerOnNewPublisherMatchedCallback(CallbackClass::jumppad, &cbObject);

    EXPECT_CALL(cbObject, callback).Times(0);

    agent.onNewPublisher(bestEffortWriterData);
}

TEST_F(AnSEDPAgent, callsPublisherCallbackIfMatched){
    CallbackClass cbObject;
    NiceMock<ReaderMock> reader(bestEffortReaderData);
    someParticipant.addReader(&reader); // Required because of dependency(search for match)
    agent.addReader(reader);

    agent.onNewPublisher(bestEffortWriterData); // Shall not crash and ignore it

    agent.registerOnNewPublisherMatchedCallback(CallbackClass::jumppad, &cbObject);
    EXPECT_CALL(cbObject, callback).Times(1);

    agent.onNewPublisher(bestEffortWriterData);
}

TEST_F(AnSEDPAgent, ignoresCallbackIfNotRegistered){
    CallbackClass cbObject;
    NiceMock<ReaderMock> reader(bestEffortReaderData);
    someParticipant.addReader(&reader); // Required because of dependency(search for match)
    agent.addReader(reader);
    NiceMock<WriterMock> writer(bestEffortReaderData);
    someParticipant.addWriter(&writer); // Required because of dependency(search for match)
    agent.addWriter(writer);

    agent.onNewPublisher(bestEffortWriterData); // Shall not crash and ignore it
    agent.onNewSubscriber(bestEffortReaderData); // Shall not crash and ignore it
}

TEST_F(AnSEDPAgent, doesntCallSubscriberCallbackIfNothingMatched){
    CallbackClass cbObject;

    agent.registerOnNewSubscriberMatchedCallback(CallbackClass::jumppad, &cbObject);
    EXPECT_CALL(cbObject, callback).Times(0);

    agent.onNewSubscriber(bestEffortReaderData);
}

TEST_F(AnSEDPAgent, callsSubscriberCallbackIfMatched){
    CallbackClass cbObject;
    NiceMock<WriterMock> writer(bestEffortReaderData);
    someParticipant.addWriter(&writer); // Required because of dependency(search for match)
    agent.addWriter(writer);

    agent.onNewSubscriber(bestEffortReaderData); // Shall not crash and ignore it

    agent.registerOnNewSubscriberMatchedCallback(CallbackClass::jumppad, &cbObject);
    EXPECT_CALL(cbObject, callback).Times(1);

    agent.onNewSubscriber(bestEffortReaderData);
}



