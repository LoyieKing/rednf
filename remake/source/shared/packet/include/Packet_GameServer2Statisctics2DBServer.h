//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAMESERVER2STATISCTICS2DBSERVER_H
#define PACKET_GAMESERVER2STATISCTICS2DBSERVER_H

#include "PacketHeader.h"
#include <string.h>

class Packet_GameServer2Statisctics2DBServer : public PacketHeader {
public:
    short m_fieldA;  // +a
    unsigned char m_fieldB;  // +c
    int m_fieldC;  // +d
    int m_fieldD;  // +11
    char m_field15[0x10];  // +15..+24（ctor memset 区域）
    char m_field25[0x10];  // +25..+34（ctor memset 区域）

    Packet_GameServer2Statisctics2DBServer() : PacketHeader(0x27fd, 0x35),
        m_fieldA(0), m_fieldB(255), m_fieldC(0), m_fieldD(0)
    {
    memset(m_field15, 0, sizeof(m_field15));
    memset(m_field25, 0, sizeof(m_field25));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_GameServer2Statisctics2DBServer, 0x35);


#endif //PACKET_GAMESERVER2STATISCTICS2DBSERVER_H
