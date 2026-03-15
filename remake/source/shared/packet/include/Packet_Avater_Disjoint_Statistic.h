//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_AVATER_DISJOINT_STATISTIC_H
#define PACKET_AVATER_DISJOINT_STATISTIC_H

#include "PacketHeader.h"

class Packet_Avater_Disjoint_Statistic : public PacketHeader {
public:
    Packet_Avater_Disjoint_Statistic(): PacketHeader(0x17a2,0x28) {};
};


#endif //PACKET_AVATER_DISJOINT_STATISTIC_H
