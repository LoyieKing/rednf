//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CIRCULATION_STATISTIC_H
#define PACKET_CIRCULATION_STATISTIC_H

#include "PacketHeader.h"

class Packet_Circulation_Statistic : public PacketHeader {
public:
    Packet_Circulation_Statistic() : PacketHeader(0x17cb, 0xd0){};
};

#endif  // PACKET_CIRCULATION_STATISTIC_H
