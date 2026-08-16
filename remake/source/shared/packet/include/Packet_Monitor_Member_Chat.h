//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_CHAT_H
#define PACKET_MONITOR_MEMBER_CHAT_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Member_Chat : public PacketHeader {
public:
    unsigned int m_memberId;        // +0xa FindUser_CharNo
    unsigned char m_msgLen;         // +0xe
    char m_msg[0x100];              // +0xf
    Packet_Monitor_Member_Chat() : PacketHeader(0x4bf, 0x10f),
        m_memberId(0), m_msgLen(0)
    {
    memset(m_msg, 0, sizeof(m_msg));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Member_Chat, 0x10f);

#endif  // PACKET_MONITOR_MEMBER_CHAT_H
