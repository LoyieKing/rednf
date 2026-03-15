//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_EMBLEM_CREATE_STATISTIC_H
#define PACKET_EMBLEM_CREATE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Emblem_Create_Statistic : public PacketHeader {
public:
    Packet_Emblem_Create_Statistic() : PacketHeader(0x17a4, 0x36){};
};

#endif  // PACKET_EMBLEM_CREATE_STATISTIC_H
