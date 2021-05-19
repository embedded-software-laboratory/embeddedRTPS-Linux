/*
 *
 * Author: Clemens Bönnen (clemens.boennen@rwth-aachen.de)
 */

#ifndef MCAST_UTILS_H
#define MCAST_UTILS_H

#include <string>

namespace measure {
    enum MeasurementPacketType : uint8_t {
        MEA_INVALID = 0,
        MEA_SEND_MULTICAST = 1,
        MEA_SEND_UNICAST = 2,
        MEA_RESPONSE_MULTICAST = 3,
        MEA_RESPONSE_UNICAST = 4,
        MEA_INIT = 5
    };
}

#endif /* MCAST_UTILS_H */