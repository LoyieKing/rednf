//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEGAPHONE_HYPER_LINK_H
#define PACKET_MONITOR_MEGAPHONE_HYPER_LINK_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_MegaPhone_Hyper_Link : public PacketHeader {
public:
    char m_fieldA;  // +a
    char m_fieldB;  // +b
    short m_fieldC;  // +c
    char m_pade[0x1];  // +e..+e（ctor 未触碰/布局待定）
    char m_fieldD;  // +f
    char m_pad10[0x1d];  // +10..+2c（ctor 未触碰/布局待定）
    char m_fieldE;  // +2d
    char m_field2e[0x68];  // +2e..+95（ctor memset 区域）
    char m_pad96[0xd0];  // +96..+165（ctor 未触碰/布局待定）
    char m_fieldF;  // +166
    char m_fieldG;  // +167
    char m_pad168[0xff];  // +168..+266（ctor 未触碰/布局待定）

    Packet_Monitor_MegaPhone_Hyper_Link() : PacketHeader(0x271b, 0x267),
        m_fieldA(0), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0), m_fieldF(0), m_fieldG(0)
    {
    memset(m_field2e, 0, sizeof(m_field2e));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_MegaPhone_Hyper_Link, 0x267);


#endif //PACKET_MONITOR_MEGAPHONE_HYPER_LINK_H
