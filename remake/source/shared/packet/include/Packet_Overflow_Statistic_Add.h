//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_OVERFLOW_STATISTIC_ADD_H
#define PACKET_OVERFLOW_STATISTIC_ADD_H

#include "PacketHeader.h"

class Packet_Overflow_Statistic_Add : public PacketHeader {
public:
    Packet_Overflow_Statistic_Add() : PacketHeader(0xc43, 0xd){};
};

#endif  // PACKET_OVERFLOW_STATISTIC_ADD_H
