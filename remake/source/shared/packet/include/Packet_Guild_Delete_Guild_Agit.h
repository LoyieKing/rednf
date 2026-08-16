//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_DELETE_GUILD_AGIT_H
#define PACKET_GUILD_DELETE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Delete_Guild_Agit : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    Packet_Guild_Delete_Guild_Agit() : PacketHeader(0x6de, 0x12),
        m_guildKey(0), m_charNo(0)
    {

    };
} __attribute__((packed));

#endif  // PACKET_GUILD_DELETE_GUILD_AGIT_H
