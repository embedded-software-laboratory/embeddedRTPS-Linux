/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de), Clemens Boennen (clemens.boennen@rwth-aachen.de)
 */

#ifndef RTPS_RECEIVER_H
#define RTPS_RECEIVER_H

#include "rtps/entities/Domain.h"


#include <vector>

namespace rtps {
    namespace tests {

        class Receiver {
        public:
            explicit Receiver(uint32_t maxMsgSizeInBytes);

            void run();

        private:
            Domain m_domain;
            Reader* mp_dataReader;

            std::vector<uint8_t> m_buffer;

            void prepareRTPS();
            static void responderJumppad(void* vp_writer, const rtps::ReaderCacheChange& cacheChange);
            void responderCallback(const rtps::ReaderCacheChange& cacheChange);
        };
    }
}

#endif //RTPS_RECEIVER_H
