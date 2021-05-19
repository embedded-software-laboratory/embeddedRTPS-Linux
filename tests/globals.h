//
// Created by Andreas Wüstenberg on 16.01.19.
//

#ifndef RTPS_GLOBALS_H
#define RTPS_GLOBALS_H

#include "rtps/common/types.h"
#include "rtps/common/Locator.h"
#include "rtps/utils/udpUtils.h"
#include "rtps/entities/ReaderProxy.h"
#include "rtps/discovery/TopicData.h"

namespace TestGlobals{
    const rtps::GuidPrefix_t someGuidPrefix{1,2,3,4,5,6,7,8,9,10,11, 1};
    const rtps::GuidPrefix_t someRemoteGuidPrefix{1,2,3,4,5,6,7,8,9,10,12, 1};
    const rtps::GuidPrefix_t someRemoteGuidPrefix2{1,2,3,4,5,6,7,8,9,10,13, 1};
    const rtps::EntityId_t someWriterId{{1,0,1}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    const rtps::EntityId_t someWriterId2{{1,0,2}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    const rtps::EntityId_t someReliableWriterId{{1,0,3}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    const rtps::EntityId_t someBestEffortWriterId{{1,0,4}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    const rtps::Guid_t someWriterGuid{someGuidPrefix, someWriterId};
    const rtps::Guid_t someWriterGuid2{someGuidPrefix, someWriterId2};
    const rtps::EntityId_t someReaderId{{2,0,1}, rtps::EntityKind_t::USER_DEFINED_READER_WITHOUT_KEY};
    const rtps::EntityId_t someReaderId2{{2,0,2}, rtps::EntityKind_t::USER_DEFINED_READER_WITHOUT_KEY};
    const rtps::EntityId_t someReliableReaderId{{2,0,3}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    const rtps::EntityId_t someBestEffortReaderId{{2,0,4}, rtps::EntityKind_t::USER_DEFINED_WRITER_WITHOUT_KEY};
    const rtps::Guid_t someReaderGuid{someGuidPrefix, someReaderId};
    const rtps::Guid_t someReaderGuid2{someGuidPrefix, someReaderId2};
    const rtps::Guid_t someRemoteReaderGuid{someRemoteGuidPrefix, someReaderId};
    const rtps::Locator someUserUnicastLocator = rtps::Locator::createUDPv4Locator(192,168,0,42, rtps::getUserUnicastPort(0));
    const rtps::Locator someUserUnicastLocator2 = rtps::Locator::createUDPv4Locator(192,168,0,49, rtps::getUserUnicastPort(0));
    const rtps::Locator someMulticastLocator = rtps::Locator::createUDPv4Locator(239,255,0,2, rtps::getUserMulticastPort());
    const rtps::ReaderProxy someReaderProxy(someReaderGuid, someUserUnicastLocator);
    const rtps::ReaderProxy someReaderProxy2(someReaderGuid2, someUserUnicastLocator);

    const rtps::ReaderProxy someRemoteReader(someRemoteReaderGuid, someUserUnicastLocator);

    const rtps::ReaderProxy someMulticastReaderProxy(someReaderGuid, someUserUnicastLocator, someMulticastLocator);
    const rtps::ReaderProxy someMulticastReaderProxy2(someReaderGuid2, someUserUnicastLocator2, someMulticastLocator);

    const rtps::TopicData reliableReaderData{{someGuidPrefix,someReliableReaderId}, rtps::ReliabilityKind_t::RELIABLE, someUserUnicastLocator};
    const rtps::TopicData bestEffortReaderData{{someGuidPrefix,someBestEffortReaderId}, rtps::ReliabilityKind_t::BEST_EFFORT, someUserUnicastLocator};
    const rtps::TopicData reliableWriterData{{someGuidPrefix,someReliableWriterId}, rtps::ReliabilityKind_t::RELIABLE, someUserUnicastLocator};
    const rtps::TopicData bestEffortWriterData{{someGuidPrefix,someBestEffortWriterId}, rtps::ReliabilityKind_t::BEST_EFFORT, someUserUnicastLocator};

    const rtps::ParticipantProxyData someRemoteParticipantProxy(rtps::Guid_t{someRemoteGuidPrefix, rtps::ENTITYID_BUILD_IN_PARTICIPANT});
    const rtps::ParticipantProxyData someRemoteParticipantProxy2(rtps::Guid_t{someRemoteGuidPrefix2, rtps::ENTITYID_BUILD_IN_PARTICIPANT});
}

#endif //RTPS_GLOBALS_H
