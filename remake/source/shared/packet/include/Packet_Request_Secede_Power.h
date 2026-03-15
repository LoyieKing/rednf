//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_SECEDE_POWER_H
#define PACKET_REQUEST_SECEDE_POWER_H

#include "PacketHeader.h"

class Packet_Request_Secede_Power : public PacketHeader {
public:
    Packet_Request_Secede_Power() : PacketHeader(0x6a9, 0x13){};
};

#endif  // PACKET_REQUEST_SECEDE_POWER_H
