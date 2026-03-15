//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CUBE_STATISTIC_H
#define PACKET_CUBE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Cube_Statistic : public PacketHeader {
public:
    Packet_Cube_Statistic() : PacketHeader(0xc33, 0x17){};
};

#endif  // PACKET_CUBE_STATISTIC_H
