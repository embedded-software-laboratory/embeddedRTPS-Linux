/*
 *   
 *  Author: Clemens Boennen  (clemens.boennen@rwth-aachen.de)
 */

#ifndef MCAST_SENDER_H
#define MCAST_SENDER_H

#include <vector>
#include <chrono>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <condition_variable>

#include "rtps/entities/Domain.h"
#include "rtps/utils/Lock.h"

#include "McastOptions.h"
#include "McastUtils.h"
#include "MeasurementPacket.h"

namespace rtps {
    namespace MulticastTests {

        class MulticastSender {
        public:
            MulticastSender(bool useMulticast, bool reliable);

            void run();

        private:
            Domain m_domain;
            Reader* mp_dataReader;
            Writer* mp_dataWriter;

            bool m_multicast;
            bool m_reliable;
            uint8_t numberOfResponses;
            uint8_t numberOfPartners;
            uint32_t currentId;
            sys_mutex_t m_mutex;
            sys_sem_t m_sem;

            #ifdef USE_INTERNAL_CLOCK
            std::chrono::steady_clock::time_point m_time;
            std::vector<double> m_rtimes;
            #endif

            std::vector<uint8_t> m_buffer;

            void prepareRTPS();
            static void responderJumppad(void* vp_writer, const rtps::ReaderCacheChange& cacheChange);
            void responderCallback(const rtps::ReaderCacheChange& cacheChange);
            void start_time();
            void buildAndSendPacket(uint32_t index);
            void sendInitPacket();
            void evaluate();
        };
    }
}

#endif /* MCAST_SENDER_H */