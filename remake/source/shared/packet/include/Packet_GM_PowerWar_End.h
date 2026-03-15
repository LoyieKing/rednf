//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GM_POWERWAR_END_H
#define PACKET_GM_POWERWAR_END_H

#include "PacketHeader.h"

class Packet_GM_PowerWar_End : public PacketHeader {
public:
    Packet_GM_PowerWar_End() : PacketHeader(0x6a7, 0xa){};
};

#endif  // PACKET_GM_POWERWAR_END_H
