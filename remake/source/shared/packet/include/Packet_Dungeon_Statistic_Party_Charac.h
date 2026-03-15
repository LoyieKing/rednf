//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H
#define PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H

#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party_Charac : public PacketHeader {
public:
    Packet_Dungeon_Statistic_Party_Charac(): PacketHeader(0xc39,0x47) {};
};


#endif //PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H
