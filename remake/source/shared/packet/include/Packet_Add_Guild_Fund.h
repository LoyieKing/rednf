//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ADD_GUILD_FUND_H
#define PACKET_ADD_GUILD_FUND_H

#include "PacketHeader.h"

class Packet_Add_Guild_Fund : public PacketHeader {
public:
    unsigned char m_mode;           // +0xa
    unsigned int m_fund;            // +0xb
    unsigned int m_charNo;          // +0xf
    unsigned int m_guildKey;        // +0x13
    Packet_Add_Guild_Fund() : PacketHeader(0x1bbc, 0x17),
        m_mode(0), m_fund(0), m_charNo(0), m_guildKey(0)
    {

    };
} __attribute__((packed));

#endif  // PACKET_ADD_GUILD_FUND_H
