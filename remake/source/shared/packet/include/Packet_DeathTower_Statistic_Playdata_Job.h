//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H
#define PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Playdata_Job : public PacketHeader {
public:
    Packet_DeathTower_Statistic_Playdata_Job(): PacketHeader(0xc3d,0x16) {};
};


#endif //PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H
