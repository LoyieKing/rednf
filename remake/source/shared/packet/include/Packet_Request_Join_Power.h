//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_JOIN_POWER_H
#define PACKET_REQUEST_JOIN_POWER_H

#include "PacketHeader.h"

class Packet_Request_Join_Power : public PacketHeader {
public:
    Packet_Request_Join_Power() : PacketHeader(0x6a8, 0x13){};
};

#endif  // PACKET_REQUEST_JOIN_POWER_H
