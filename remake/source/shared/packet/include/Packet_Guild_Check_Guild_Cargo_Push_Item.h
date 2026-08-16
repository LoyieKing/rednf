//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H
#define PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Check_Guild_Cargo_Push_Item : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    int m_field12;              // +0x12
    int m_field16;              // +0x16
    int m_field1a;              // +0x1a
    int m_field1e;              // +0x1e
    unsigned char m_field22;    // +0x22
    unsigned char m_field23;    // +0x23
    unsigned short m_field24;   // +0x24
    Packet_Guild_Check_Guild_Cargo_Push_Item() : PacketHeader(0x70c, 0x26),
        m_guildKey(0), m_charNo(0), m_field12(0), m_field16(0), m_field1a(0), m_field1e(0), m_field22(0), m_field23(0), m_field24(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H
