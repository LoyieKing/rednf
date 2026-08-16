//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_DEBUG_H
#define PACKET_GUILD_DEBUG_H

#include "PacketHeader.h"

class Packet_Guild_Debug : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    int m_mode;               // +0x12
    int m_field16;            // +0x16（ctor 清零）
    int m_field1a;            // +0x1a（ctor 清零）

    Packet_Guild_Debug() : PacketHeader(0x1bc9, 0x1e),
        m_guildKey(0), m_charNo(0), m_mode(0), m_field16(0), m_field1a(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Debug, 0x1e);

#endif  // PACKET_GUILD_DEBUG_H
