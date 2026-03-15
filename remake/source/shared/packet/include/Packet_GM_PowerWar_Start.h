//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GM_POWERWAR_START_H
#define PACKET_GM_POWERWAR_START_H

#include "PacketHeader.h"

class Packet_GM_PowerWar_Start : public PacketHeader {
public:
    Packet_GM_PowerWar_Start() : PacketHeader(0x6a6, 0xa){};
};

#endif  // PACKET_GM_POWERWAR_START_H
