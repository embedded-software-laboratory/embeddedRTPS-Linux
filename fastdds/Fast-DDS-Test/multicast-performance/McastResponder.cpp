// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file McastResponder.cpp
 *
 */

#include "McastResponder.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/Domain.h>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

#define MCAST_RESPONDER_VERBOSE 0

McastResponder::McastResponder(bool multicast, bool reliable)
    : mp_participant(nullptr)
    , mp_subscriber(nullptr)
    , m_multicast(multicast)
    , m_reliable(reliable)
    , m_listener(*this)
{ }

bool McastResponder::init()
{
    eprosima::fastrtps::rtps::Locator_t mlocator;
    IPLocator::setIPv4(mlocator, 239, 255, 0, 2);
    mlocator.port = 7401;

    eprosima::fastrtps::rtps::Locator_t ulocator;
    IPLocator::setIPv4(ulocator, 192, 168, 1, 1);
    ulocator.port = 7413;

    ParticipantAttributes PParam;
    PParam.rtps.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
    PParam.rtps.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_sub");
    mp_participant = Domain::createParticipant(PParam);
    if (mp_participant == nullptr)
    {
        return false;
    }

    //REGISTER THE TYPE

    Domain::registerType(mp_participant, &m_type);
    //CREATE THE SUBSCRIBER
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = "PerformanceType";
    Rparam.topic.topicName = "MulticastPerf";
    Rparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Rparam.topic.historyQos.depth = 30;
    Rparam.topic.resourceLimitsQos.max_samples = 50;
    Rparam.topic.resourceLimitsQos.allocated_samples = 20;
    if(m_reliable) {
        Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    } else {
        Rparam.qos.m_reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
    }
    Rparam.qos.m_durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;
    if(m_multicast) {
        Rparam.multicastLocatorList.push_back(mlocator);
    }
    Rparam.unicastLocatorList.push_back(ulocator);
    mp_subscriber = Domain::createSubscriber(mp_participant, Rparam, (SubscriberListener*)&m_listener);

    if (mp_subscriber == nullptr)
    {
        return false;
    }

    //CREATE THE PUBLISHER
    PublisherAttributes Wparam;
    Wparam.topic.topicKind = NO_KEY;
    Wparam.topic.topicDataType = "PerformanceType";
    Wparam.topic.topicName = "MulticastPerfResp";
    Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Wparam.topic.historyQos.depth = 30;
    Wparam.topic.resourceLimitsQos.max_samples = 50;
    Wparam.topic.resourceLimitsQos.allocated_samples = 20;
    Wparam.times.heartbeatPeriod.seconds = 2;
    Wparam.times.heartbeatPeriod.nanosec = 200 * 1000 * 1000;
    if(m_reliable) {
        Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    } else {
        Wparam.qos.m_reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
    }
    mp_publisher = Domain::createPublisher(mp_participant, Wparam, (PublisherListener*)&m_listener);
    if (mp_publisher == nullptr)
    {
        return false;
    }

    return true;
}

bool McastResponder::getMulticast() {
    return m_multicast;
}

McastResponder::~McastResponder()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant);
}

void McastResponder::SubListener::onSubscriptionMatched(
        Subscriber* /*sub*/,
        MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "[ INFO  ] Subscriber matched." << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "[ INFO  ] Subscriber unmatched." << std::endl;
    }
}

void McastResponder::SubListener::onNewDataMessage(
        Subscriber* sub)
{
    if (sub->takeNextData((void*)&m_mpack, &m_info))
    {
        if (m_info.sampleKind == ALIVE)
        {
            this->n_samples++;
            // Print your structure data here.
            if (m_mpack.p_type() != measure::MEA_INIT) {
                if(m_responder.getMulticast()){
                    m_mpack.p_type(measure::MEA_RESPONSE_MULTICAST);
                } else {
                    m_mpack.p_type(measure::MEA_RESPONSE_UNICAST);
                }
            }
            #if MCAST_RESPONDER_VERBOSE
            std::cout << "[ INFO  ] Message received. ID: " << m_mpack.id() << " Sending response." << std::endl;
            #endif
            m_responder.mp_publisher->write((void*)&m_mpack);
        }
    }

}

void McastResponder::run()
{
    std::cout << "[ INFO  ] Subscriber running..." << std::endl;
    while(true);
}

void McastResponder::run(
        uint32_t number)
{
    std::cout << "[ INFO  ] Subscriber running until " << number << "samples have been received" << std::endl;
    while (number > this->m_listener.n_samples)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
