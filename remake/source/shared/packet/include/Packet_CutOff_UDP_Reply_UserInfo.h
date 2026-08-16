//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CUTOFF_UDP_REPLY_USERINFO_H
#define PACKET_CUTOFF_UDP_REPLY_USERINFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_CutOff_UDP_Reply_UserInfo : public PacketHeader {
public:
    short m_fieldA;  // +a
    unsigned char m_fieldB;  // +c
    char m_fieldC;  // +d
    char m_fielde[0xaf0];  // +e..+afd（ctor memset 区域）

    Packet_CutOff_UDP_Reply_UserInfo() : PacketHeader(0x3f9, 0xafe),
        m_fieldA(0), m_fieldB(255), m_fieldC(0)
    {
    memset(m_fielde, 0, sizeof(m_fielde));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_CutOff_UDP_Reply_UserInfo, 0xafe);


#endif //PACKET_CUTOFF_UDP_REPLY_USERINFO_H
