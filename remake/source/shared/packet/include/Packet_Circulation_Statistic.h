//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CIRCULATION_STATISTIC_H
#define PACKET_CIRCULATION_STATISTIC_H

#include "PacketHeader.h"

class Packet_Circulation_Statistic : public PacketHeader {
public:
    Packet_Circulation_Statistic() : PacketHeader(0x17cb, 0xd0){};
    int m_f0a;        // +0xa
    int m_data[0x30]; // +0xe（0x30 * 4 = 0xc0，总 0xce；ORIG 仅访达 0xce）
} __attribute__((packed));

#endif  // PACKET_CIRCULATION_STATISTIC_H
