//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SERVER_MESSAGE_INFO_H
#define PACKET_MONITOR_SERVER_MESSAGE_INFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Server_Message_Info : public PacketHeader {
public:
    char m_fieldA;  // +a
    char m_fieldB;  // +b
    char m_fieldC;  // +c
    int m_fieldD;  // +d
    char m_field11[0x32];  // +11..+42（ctor memset 区域）
    int m_fieldE;  // +43
    char m_field47[0x14];  // +47..+5a（ctor memset 区域）
    int m_fieldF;  // +5b

    Packet_Monitor_Server_Message_Info() : PacketHeader(0xfae, 0x5f),
        m_fieldA(0), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0), m_fieldF(0)
    {
    memset(m_field11, 0, sizeof(m_field11));
    memset(m_field47, 0, sizeof(m_field47));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_Server_Message_Info, 0x5f);


#endif //PACKET_MONITOR_SERVER_MESSAGE_INFO_H
