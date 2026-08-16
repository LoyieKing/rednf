//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEGAPHONE_H
#define PACKET_MONITOR_MEGAPHONE_H

#include "PacketHeader.h"

class Packet_Monitor_MegaPhone : public PacketHeader {
public:
    char m_fieldA;  // +a
    char m_fieldB;  // +b
    short m_fieldC;  // +c
    char m_pade[0x1];  // +e..+e（ctor 未触碰/布局待定）
    char m_fieldD;  // +f
    char m_pad10[0x1d];  // +10..+2c（ctor 未触碰/布局待定）
    char m_fieldE;  // +2d
    char m_fieldF;  // +2e
    char m_pad2f[0xff];  // +2f..+12d（ctor 未触碰/布局待定）

    Packet_Monitor_MegaPhone() : PacketHeader(0x546, 0x12e),
        m_fieldA(0), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0), m_fieldF(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_MegaPhone, 0x12e);

#endif  // PACKET_MONITOR_MEGAPHONE_H
