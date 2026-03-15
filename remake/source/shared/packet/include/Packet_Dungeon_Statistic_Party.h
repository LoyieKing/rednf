//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DUNGEON_STATISTIC_PARTY_H
#define PACKET_DUNGEON_STATISTIC_PARTY_H

#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party : public PacketHeader {
public:
    Packet_Dungeon_Statistic_Party() : PacketHeader(0xc35, 0x40){};
};

#endif  // PACKET_DUNGEON_STATISTIC_PARTY_H
