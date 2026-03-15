//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REGISTER_GM_MID_H
#define PACKET_REGISTER_GM_MID_H

#include "PacketHeader.h"

class Packet_Register_GM_MID : public PacketHeader {
public:
    Packet_Register_GM_MID() : PacketHeader(0x1f44, 0xe){};
};

#endif  // PACKET_REGISTER_GM_MID_H
