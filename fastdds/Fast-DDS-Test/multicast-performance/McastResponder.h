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
 * @file McastResponder.h
 *
 */

#ifndef MCAST_RESPONDER_H
#define MCAST_RESPONDER_H

#include "MeasurementPacketPubSubTypes.h"

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include "MeasurementPacket.h"
#include "McastUtils.h"

class McastResponder {
public:
	McastResponder(bool multicast, bool reliable);
	virtual ~McastResponder();
	//!Initialize the subscriber
	bool init();
	//!RUN the subscriber
	void run();
	//!Run the subscriber until number samples have been recevied.
	void run(uint32_t number);

	bool getMulticast();
private:
	eprosima::fastrtps::Participant* mp_participant;
	eprosima::fastrtps::Subscriber* mp_subscriber;
	eprosima::fastrtps::Publisher* mp_publisher;
public:
	class SubListener:public eprosima::fastrtps::SubscriberListener
	{
	public:
		SubListener(McastResponder& responder):n_matched(0),n_samples(0), m_responder(responder){};
		~SubListener(){};
		void onSubscriptionMatched(eprosima::fastrtps::Subscriber* sub, eprosima::fastrtps::rtps::MatchingInfo& info);
		void onNewDataMessage(eprosima::fastrtps::Subscriber* sub);
		MeasurementPacket m_mpack;
		eprosima::fastrtps::SampleInfo_t m_info;
		int n_matched;
		uint32_t n_samples;
	private:
		McastResponder& m_responder;
	}m_listener;
private:
	MeasurementPacketPubSubType m_type;
	bool m_multicast;
	bool m_reliable;
};

#endif /* MCAST_RESPONDER_H */
