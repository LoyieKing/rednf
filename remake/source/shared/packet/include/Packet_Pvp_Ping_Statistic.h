//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVP_PING_STATISTIC_H
#define PACKET_PVP_PING_STATISTIC_H

#include "PacketHeader.h"

class Packet_Pvp_Ping_Statistic : public PacketHeader {
public:
    Packet_Pvp_Ping_Statistic() : PacketHeader(0xfa8, 0xe){};
};

#endif  // PACKET_PVP_PING_STATISTIC_H
