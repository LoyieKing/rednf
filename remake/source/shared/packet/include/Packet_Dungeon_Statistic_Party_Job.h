//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DUNGEON_STATISTIC_PARTY_JOB_H
#define PACKET_DUNGEON_STATISTIC_PARTY_JOB_H

#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party_Job : public PacketHeader {
public:
    Packet_Dungeon_Statistic_Party_Job(): PacketHeader(0xc37,0x1f) {};
};


#endif //PACKET_DUNGEON_STATISTIC_PARTY_JOB_H
