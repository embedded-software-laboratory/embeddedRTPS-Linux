/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de), Clemens Boennen (clemens.boennen@rwth-aachen.de)
 */

#ifndef RTPS_SENDER_H
#define RTPS_SENDER_H

#include <rtps/entities/Domain.h>

#include "rtps/config.h"

#include <chrono>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace rtps{
    namespace tests{

        class Sender{
        public:
            Sender(uint32_t numSamples);
            void run();

        private:
            Domain m_domain;
            Writer* mp_dataWriter;

            uint32_t m_numSamples;
            uint32_t m_index;

            void prepareRTPS();
            void runWithIndex(uint32_t index);
        };
    }
}

#endif //RTPS_SENDER_H
