//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VALUE_STATISTIC_H
#define PACKET_VALUE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Value_Statistic : public PacketHeader {
public:
    Packet_Value_Statistic() : PacketHeader(0x17ca, 0x86){};
};

#endif  // PACKET_VALUE_STATISTIC_H
