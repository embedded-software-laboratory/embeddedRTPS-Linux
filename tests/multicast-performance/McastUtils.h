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

const char rands[128] = "GfemHjbEIYnhUaROYqbNFKxBEGYXdvOYXxoSvBfktOwSmRROTDAnJjahmyDLgawTmDUsaQzIoaLZQmBoJHfIkZgnvvFdmyzKJpwKkFfhpyIvDdcFCUIPLIDNJZZQiVb";
static char stri[255];


inline const char* getRandomString(uint32_t length, uint8_t offset) {
    for (uint32_t i = 0; i < length-1; i++) {
        stri[i] = rands[(offset+i)%127];
    }
    stri[length-1] = '\0';
    return stri;
}

#endif /* MCAST_UTILS_H */