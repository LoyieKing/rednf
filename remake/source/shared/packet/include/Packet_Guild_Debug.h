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
    Packet_Guild_Debug() : PacketHeader(0x1bc9, 0x1e){};
} __attribute__((packed));

#endif  // PACKET_GUILD_DEBUG_H
