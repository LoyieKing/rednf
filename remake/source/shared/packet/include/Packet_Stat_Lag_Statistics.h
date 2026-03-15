//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_STAT_LAG_STATISTICS_H
#define PACKET_STAT_LAG_STATISTICS_H

#include "PacketHeader.h"

class Packet_Stat_Lag_Statistics : public PacketHeader {
public:
    Packet_Stat_Lag_Statistics() : PacketHeader(0x17b9, 0x14c){};
};

#endif  // PACKET_STAT_LAG_STATISTICS_H
