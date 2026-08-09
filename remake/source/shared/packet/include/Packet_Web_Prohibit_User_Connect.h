//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_PROHIBIT_USER_CONNECT_H
#define PACKET_WEB_PROHIBIT_USER_CONNECT_H

#include "PacketHeader.h"

class Packet_Web_Prohibit_User_Connect : public PacketHeader {
public:
    Packet_Web_Prohibit_User_Connect() : PacketHeader(0x4c8, 0x13)
    {
        m_fieldA = 0;
        m_fieldF = 0;
        m_field11 = 0;
        m_field12 = 0;
    }
    int m_fieldA;       // +0xa
    char m_fieldE;      // +0xe
    unsigned short m_fieldF;  // +0xf
    char m_field11;     // +0x11
    char m_field12;     // +0x12
} __attribute__((packed));

#endif  // PACKET_WEB_PROHIBIT_USER_CONNECT_H
