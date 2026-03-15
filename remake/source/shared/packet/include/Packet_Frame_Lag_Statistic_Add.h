//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FRAME_LAG_STATISTIC_ADD_H
#define PACKET_FRAME_LAG_STATISTIC_ADD_H

#include "PacketHeader.h"

class Packet_Frame_Lag_Statistic_Add : public PacketHeader {
public:
    Packet_Frame_Lag_Statistic_Add() : PacketHeader(0xc26, 0x186){};
};

#endif  // PACKET_FRAME_LAG_STATISTIC_ADD_H
