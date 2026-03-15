//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H
#define PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H

#include "PacketHeader.h"

class Packet_TowerOfDespair_Statistic_GTS : public PacketHeader {
public:
    Packet_TowerOfDespair_Statistic_GTS(): PacketHeader(0x271c,0x17) {};
};


#endif //PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H
