/*
 *   
 *  Author: Clemens Boennen  (clemens.boennen@rwth-aachen.de)
 */

#ifndef MCAST_RECEIVER_H
#define MCAST_RECEIVER_H

#include <vector>

#include "rtps/entities/Domain.h"

#include "McastOptions.h"
#include "MeasurementPacket.h"
#include "McastUtils.h"

namespace rtps {
    namespace MulticastTests {

        class MulticastReceiver {
        public:
            MulticastReceiver(bool useMulticast, bool reliable);

            void run();

        private:
            Domain m_domain;
            Reader* mp_dataReader;
            Writer* mp_dataWriter;

            bool m_multicast;
            bool m_reliable;

            std::vector<uint8_t> m_buffer;

            void prepareRTPS();
            static void responderJumppad(void* vp_writer, const rtps::ReaderCacheChange& cacheChange);
            void responderCallback(const rtps::ReaderCacheChange& cacheChange);
            void sendResponse(MeasurementPacket mp);
        };
    }
}

#endif /* MCAST_RECEIVER_H */