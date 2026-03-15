//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DEATHTOWER_STATISTIC_VALUE_H
#define PACKET_DEATHTOWER_STATISTIC_VALUE_H

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Value : public PacketHeader {
public:
    Packet_DeathTower_Statistic_Value(): PacketHeader(0xc3b,0x15) {};
};


#endif //PACKET_DEATHTOWER_STATISTIC_VALUE_H
