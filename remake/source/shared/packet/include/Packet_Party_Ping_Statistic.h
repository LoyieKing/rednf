//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PARTY_PING_STATISTIC_H
#define PACKET_PARTY_PING_STATISTIC_H

#include "PacketHeader.h"

class Packet_Party_Ping_Statistic : public PacketHeader {
public:
    Packet_Party_Ping_Statistic() : PacketHeader(0xfa7, 0xe){};
};

#endif  // PACKET_PARTY_PING_STATISTIC_H
