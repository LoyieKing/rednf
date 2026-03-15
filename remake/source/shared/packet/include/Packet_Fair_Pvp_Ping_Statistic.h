//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FAIR_PVP_PING_STATISTIC_H
#define PACKET_FAIR_PVP_PING_STATISTIC_H

#include "PacketHeader.h"

class Packet_Fair_Pvp_Ping_Statistic : public PacketHeader {
public:
    Packet_Fair_Pvp_Ping_Statistic() : PacketHeader(0x27ed, 0xe){};
};

#endif  // PACKET_FAIR_PVP_PING_STATISTIC_H
