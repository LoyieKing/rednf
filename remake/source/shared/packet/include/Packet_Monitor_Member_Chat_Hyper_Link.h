//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_MEMBER_CHAT_HYPER_LINK_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Member_Chat_Hyper_Link : public PacketHeader {
public:
    unsigned int m_memberId;        // +0xa  FindUser_CharNo / FindMember
    unsigned char m_itemCount;      // +0xe
    char m_items[3][0x68];          // +0xf
    unsigned char m_msgLen;         // +0x147
    char m_msg[0x100];              // +0x148
    Packet_Monitor_Member_Chat_Hyper_Link() : PacketHeader(0x271a, 0x248),
        m_memberId(0), m_itemCount(0), m_msgLen(0)
    {
    // ORIG 0x086d1cc0：具名字段直存 `movl $0x0,0xa(%eax)`（裸偏移写反产 add+存）；
    // 0x086d1cdb memset(this+0x148,0x100) 先、0x086d1cfb memset(this+0xf,0x138) 后
    m_memberId = 0;
    m_itemCount = 0;
    m_msgLen = 0;
    memset(m_msg, 0, sizeof(m_msg));
    memset(m_items, 0, sizeof(m_items));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Member_Chat_Hyper_Link, 0x248);

#endif //PACKET_MONITOR_MEMBER_CHAT_HYPER_LINK_H
