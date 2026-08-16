//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_EMBLEM_CREATE_STATISTIC_H
#define PACKET_EMBLEM_CREATE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Emblem_Create_Statistic : public PacketHeader {
public:
    Packet_Emblem_Create_Statistic() : PacketHeader(0x17a4, 0x36),
        m_count(0)
    {

    };
    int m_count;   // +0xa
    int m_arrB[5]; // +0xe..0x22
    int m_arrA[5]; // +0x22..0x36（ORIG 用 m_arrA[i+8]，GCC 折叠为 add $8 + 0x2(...,4)）
} __attribute__((packed));

#endif  // PACKET_EMBLEM_CREATE_STATISTIC_H
