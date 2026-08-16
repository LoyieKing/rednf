//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CUBE_STATISTIC_H
#define PACKET_CUBE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Cube_Statistic : public PacketHeader {
public:
    short m_fieldA;  // +a
    short m_fieldB;  // +c
    int m_fieldC;  // +e
    int m_fieldD;  // +12
    char m_fieldE;  // +16

    Packet_Cube_Statistic() : PacketHeader(0xc33, 0x17),
        m_fieldA(0), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Cube_Statistic, 0x17);

#endif  // PACKET_CUBE_STATISTIC_H
