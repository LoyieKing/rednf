//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_USERSTATUS_H
#define PACKET_HADES_USERSTATUS_H

#include "PacketHeader.h"

class Packet_Hades_UserStatus : public PacketHeader {
public:
    Packet_Hades_UserStatus() : PacketHeader(0xfdd, 0x12){};
};

#endif  // PACKET_HADES_USERSTATUS_H
