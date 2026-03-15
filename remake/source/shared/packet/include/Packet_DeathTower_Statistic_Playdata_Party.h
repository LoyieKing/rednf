//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H
#define PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Playdata_Party : public PacketHeader {
public:
    Packet_DeathTower_Statistic_Playdata_Party(): PacketHeader(0xc3f,0x10) {};
};


#endif //PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H
