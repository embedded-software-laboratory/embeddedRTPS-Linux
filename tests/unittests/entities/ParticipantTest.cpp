/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rtps/entities/Participant.h"
#include "rtps/common/types.h"

#include "../../mocking/ReaderMock.h"
#include "../../mocking/WriterMock.h"
#include "../../globals.h"

using rtps::Participant;

class SomeParticipant : public ::testing::Test{
protected:
    rtps::ParticipantId_t someId = 1;
    Participant part{TestGlobals::someGuidPrefix, someId};
};

TEST_F(SomeParticipant, getNextUserEntityKey_increasesCorrectly){
    auto zeroKey = part.getNextUserEntityKey();

    ASSERT_THAT(zeroKey, testing::ElementsAre(0,0,1));

    auto nextKey = part.getNextUserEntityKey();

    ASSERT_THAT(nextKey, testing::ElementsAre(0,0,2));
}

TEST_F(SomeParticipant, readerCanBeFoundByEntityIdAfterAdding){
    rtps::EntityId_t someEntityId{{1,2,3}, rtps::EntityKind_t::USER_DEFINED_READER_WITHOUT_KEY};
    ReaderMock mock{{TestGlobals::someGuidPrefix, someEntityId}};

    rtps::Reader* returnedReader = part.addReader(&mock);
    EXPECT_NE(returnedReader, nullptr);

    rtps::Reader* foundReader = part.getReader(someEntityId);
    EXPECT_EQ(foundReader, returnedReader);
}

TEST_F(SomeParticipant, withReliableReaderMatchesReliableWrtier){
    ReaderMock reader(TestGlobals::reliableReaderData);
    part.addReader(&reader);

    rtps::Reader* match = part.getMatchingReader(TestGlobals::reliableWriterData);

    ASSERT_NE(match, nullptr);
    EXPECT_EQ(match, &reader);
}

TEST_F(SomeParticipant, withReliableReaderDoesntMatcheBestEffortWriter){
    ReaderMock reader(TestGlobals::reliableReaderData);
    part.addReader(&reader);

    rtps::Reader* match = part.getMatchingReader(TestGlobals::bestEffortWriterData);

    EXPECT_EQ(match, nullptr);
}

TEST_F(SomeParticipant, withBestEffortReaderMatchesReliableWrtier){
    ReaderMock reader(TestGlobals::bestEffortReaderData);
    part.addReader(&reader);

    rtps::Reader* match = part.getMatchingReader(TestGlobals::reliableWriterData);

    ASSERT_NE(match, nullptr);
    EXPECT_EQ(match, &reader);
}

TEST_F(SomeParticipant, withBestEffortReaderMatcheBestEffortWriter){
    ReaderMock reader(TestGlobals::bestEffortReaderData);
    part.addReader(&reader);

    rtps::Reader* match = part.getMatchingReader(TestGlobals::bestEffortWriterData);

    ASSERT_NE(match, nullptr);
    EXPECT_EQ(match, &reader);
}


TEST_F(SomeParticipant, withBestEffortWriterDoesntMatchesReliableReader){
    WriterMock writer(TestGlobals::bestEffortWriterData);
    part.addWriter(&writer);

    rtps::Writer* match = part.getMatchingWriter(TestGlobals::reliableReaderData);

    EXPECT_EQ(match, nullptr);
}

TEST_F(SomeParticipant, withBestEffortWriterMatchesBestEfforteReader){
    WriterMock writer(TestGlobals::bestEffortWriterData);
    part.addWriter(&writer);

    rtps::Writer* match = part.getMatchingWriter(TestGlobals::bestEffortReaderData);

    ASSERT_NE(match, nullptr);
    EXPECT_EQ(match, &writer);
}

TEST_F(SomeParticipant, findRemoteParticipant_returnsNullprt){

    auto* foundPart = part.findRemoteParticipant(TestGlobals::someRemoteGuidPrefix);

    EXPECT_EQ(foundPart, nullptr);
}

TEST_F(SomeParticipant, canFindRemoteParticipantAfterAdding){
    auto& remotePart = TestGlobals::someRemoteParticipantProxy;
   part.addNewRemoteParticipant(remotePart);

   auto* foundPart = part.findRemoteParticipant(remotePart.m_guid.prefix);

   ASSERT_NE(foundPart, nullptr);
   EXPECT_EQ(remotePart.m_guid, foundPart->m_guid);
}

TEST_F(SomeParticipant, canFindRemoteParticipantAfterAddingAnother){
    auto& remotePart1 = TestGlobals::someRemoteParticipantProxy;
    auto& remotePart2 = TestGlobals::someRemoteParticipantProxy2;
    bool success = part.addNewRemoteParticipant(remotePart1);
    ASSERT_TRUE(success);
    success = part.addNewRemoteParticipant(remotePart2);
    ASSERT_TRUE(success);

    auto* foundPart = part.findRemoteParticipant(remotePart1.m_guid.prefix);

    ASSERT_NE(foundPart, nullptr);
    EXPECT_EQ(remotePart1.m_guid, foundPart->m_guid);
}

TEST_F(SomeParticipant, removeRemoteParticipantDeletesCorrectOne){
    auto& remotePart1 = TestGlobals::someRemoteParticipantProxy;
    auto& remotePart2 = TestGlobals::someRemoteParticipantProxy2;
    bool success = part.addNewRemoteParticipant(remotePart1);
    ASSERT_TRUE(success);
    success = part.addNewRemoteParticipant(remotePart2);
    ASSERT_TRUE(success);


    success = part.removeRemoteParticipant(remotePart2.m_guid.prefix);
    ASSERT_TRUE(success);

    auto* foundPart = part.findRemoteParticipant(remotePart1.m_guid.prefix);
    ASSERT_NE(foundPart, nullptr);
    foundPart = part.findRemoteParticipant(remotePart2.m_guid.prefix);
    ASSERT_EQ(foundPart, nullptr);
}