//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_RANDOMBOX_STATISTIC_H
#define PACKET_RANDOMBOX_STATISTIC_H

#include "PacketHeader.h"

class Packet_Randombox_statistic : public PacketHeader {
public:
    char m_fieldA;  // +a
    char m_fieldB;  // +b

    Packet_Randombox_statistic() : PacketHeader(0x17b6, 0xc),
        m_fieldA(0), m_fieldB(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Randombox_statistic, 0xc);

#endif  // PACKET_RANDOMBOX_STATISTIC_H
