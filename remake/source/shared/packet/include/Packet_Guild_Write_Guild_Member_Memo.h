//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_WRITE_GUILD_MEMBER_MEMO_H
#define PACKET_GUILD_WRITE_GUILD_MEMBER_MEMO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Write_Guild_Member_Memo : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    char m_memo[0x15];        // +0x12
    Packet_Guild_Write_Guild_Member_Memo() : PacketHeader(0x4d1, 0x27),
        m_guildKey(0), m_charNo(0)
    {
    memset(m_memo, 0, sizeof(m_memo));
    };
} __attribute__((packed));


#endif //PACKET_GUILD_WRITE_GUILD_MEMBER_MEMO_H
