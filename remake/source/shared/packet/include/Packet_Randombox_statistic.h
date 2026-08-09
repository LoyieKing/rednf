//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_RANDOMBOX_STATISTIC_H
#define PACKET_RANDOMBOX_STATISTIC_H

#include "PacketHeader.h"

class Packet_Randombox_statistic : public PacketHeader {
public:
    Packet_Randombox_statistic() : PacketHeader(0x17b6, 0xc){};
} __attribute__((packed));

#endif  // PACKET_RANDOMBOX_STATISTIC_H
