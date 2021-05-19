/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#ifndef RTPS_NETWORKDRIVERMOCK_H
#define RTPS_NETWORKDRIVERMOCK_H

#include <gmock/gmock.h>

#include "rtps/communication/PacketInfo.h"

class NetworkDriverMock{
public:
    MOCK_METHOD1(sendPacket, void(rtps::PacketInfo& packetInfo));
    virtual ~NetworkDriverMock(){}
};

#endif //RTPS_NETWORKDRIVERMOCK_H
