/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include <gtest/gtest.h>

#include "rtps/config.h"
#include "rtps/entities/Domain.h"
#include "rtps/rtps.h"

using rtps::Domain;
using namespace rtps::Config;


TEST(ADomain, createParticipant_generatesDifferentGuids){
    ASSERT_TRUE(MAX_NUM_PARTICIPANTS >= 2);
    ASSERT_TRUE(NUM_STATELESS_READERS >= 2);
    ASSERT_TRUE(NUM_STATELESS_WRITERS >= 2);
    ASSERT_TRUE(NUM_STATEFUL_READERS >= 4); // Required by current implementation, not by specification
    ASSERT_TRUE(NUM_STATEFUL_WRITERS >= 4); // Required by current implementation, not by specification
    Domain domain;

    rtps::Participant* firstPart = domain.createParticipant();
    rtps::Participant* secondPart = domain.createParticipant();

    ASSERT_NE(firstPart, nullptr);
    ASSERT_NE(secondPart, nullptr);

    EXPECT_NE(firstPart->m_guidPrefix.id, secondPart->m_guidPrefix.id);
}

TEST(ADomain, cannotCreateMoreParticipantThanSlotsAvailable){
    Domain domain;
    for(size_t i=0; i < MAX_NUM_PARTICIPANTS; ++i){
        auto* part = domain.createParticipant();
        ASSERT_NE(part, nullptr);
    }

    auto* part = domain.createParticipant();
    EXPECT_EQ(part, nullptr);
}

TEST(ADomain, cannotCreateMoreEndpointsThanSlotsAvailable){
    Domain domain;
    char topicName[] = "topic";
    char typeName[] = "type";
    auto* part = domain.createParticipant();
    ASSERT_NE(part, nullptr);

    // BUILTIN ENDPOINTS
    const uint8_t statelessWritersUsed = 1;
    const uint8_t statelessReadersUsed = 1;
    const uint8_t statefulWritersUsed = 2;
    const uint8_t statefulReadersUsed = 2;

    const uint8_t availWritersPart = NUM_WRITERS_PER_PARTICIPANT - statefulWritersUsed - statelessWritersUsed;
    const uint8_t availStatefulWritersDom = NUM_STATEFUL_WRITERS - statefulWritersUsed;
    const uint8_t availStatelessWritersDom = NUM_STATELESS_WRITERS - statelessWritersUsed;

    uint8_t availStatelessWriters;
    uint8_t availStatefulWriters;

    if(availWritersPart < availStatelessWritersDom) {
        availStatelessWriters = availWritersPart;
    } else {
        availStatelessWriters = availStatelessWritersDom;
    }

    if(availWritersPart - availStatelessWriters < availStatefulWritersDom) {
        availStatefulWriters = availWritersPart - availStatelessWriters;
    } else {
        availStatefulWriters = availStatefulWritersDom;
    }

    const uint8_t availReadersPart = NUM_READERS_PER_PARTICIPANT - statefulReadersUsed - statelessReadersUsed;
    const uint8_t availStatefulReadersDom = NUM_STATEFUL_READERS - statefulReadersUsed;
    const uint8_t availStatelessReadersDom = NUM_STATELESS_READERS - statelessReadersUsed;

    uint8_t availStatelessReaders;
    uint8_t availStatefulReaders;

    if(availReadersPart < availStatelessReadersDom) {
        availStatelessReaders = availReadersPart;
    } else {
        availStatelessReaders = availStatelessReadersDom;
    }

    if(availReadersPart - availStatelessReaders < availStatefulReadersDom) {
        availStatefulReaders = availReadersPart - availStatelessReaders;
    } else {
        availStatefulReaders = availStatefulReadersDom;
    }

    rtps::Writer* writer = nullptr;
    // STATELESS WRITER
    for(size_t i=0; i < availStatelessWriters; ++i){
        writer = domain.createWriter(*part, topicName, typeName, false);
        ASSERT_NE(writer, nullptr);
    }
    writer = domain.createWriter(*part, topicName, typeName, false);
    EXPECT_EQ(writer, nullptr);

    // STATEFUL WRITER
    for(size_t i=0; i < availStatefulWriters; ++i){
        writer = domain.createWriter(*part, topicName, typeName, true);
        ASSERT_NE(writer, nullptr);
    }
    writer = domain.createWriter(*part, topicName, typeName, true);
    EXPECT_EQ(writer, nullptr);

    rtps::Reader* reader = nullptr;
    // STATELESS READER
    for(size_t i=0; i < availStatelessReaders; ++i){
        reader = domain.createReader(*part, topicName, typeName, false);
        ASSERT_NE(reader, nullptr);
    }
    reader = domain.createReader(*part, topicName, typeName, false);
    EXPECT_EQ(reader, nullptr);

    // STATEFUL READER
    for(size_t i=0; i < availStatefulReaders; ++i){
        reader = domain.createReader(*part, topicName, typeName, true);
        ASSERT_NE(reader, nullptr);
    }
    reader = domain.createReader(*part, topicName, typeName, true);
    EXPECT_EQ(reader, nullptr);
}

TEST(ADomain, CannotCreateEntitiesAfterCompleteInit){
    Domain domain;
    char topicName[] = "topic";
    char typeName[] = "type";
    auto* existing_part = domain.createParticipant();
    bool success = domain.completeInit(); // TODO bottleneck for this test
    ASSERT_TRUE(success);

    auto* part = domain.createParticipant();
    EXPECT_EQ(part, nullptr);

// This should be allowed, only participants not!
//    rtps::Writer* writer = domain.createWriter(*existing_part, topicName, typeName, false);
//    EXPECT_EQ(writer, nullptr);

//    rtps::Reader* reader = domain.createReader(*existing_part, topicName, typeName, false);
//    EXPECT_EQ(reader, nullptr);
}

