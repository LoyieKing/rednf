//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_STAT_LAG_STATISTICS_H
#define PACKET_STAT_LAG_STATISTICS_H

#include "PacketHeader.h"

class Packet_Stat_Lag_Statistics : public PacketHeader {
public:
    char m_fieldA;  // +a
    char m_padb[0x50];  // +b..+5a（ctor 未触碰/布局待定）
    char m_fieldB;  // +5b
    char m_pad5c[0xf0];  // +5c..+14b（ctor 未触碰/布局待定）

    Packet_Stat_Lag_Statistics() : PacketHeader(0x17b9, 0x14c),
        m_fieldA(0), m_fieldB(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Stat_Lag_Statistics, 0x14c);

#endif  // PACKET_STAT_LAG_STATISTICS_H
