//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_UPGRADE_GUILD_AGIT_H
#define PACKET_GUILD_UPGRADE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Upgrade_Guild_Agit : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned int m_field12;     // +0x12
    unsigned int m_fund;        // +0x16
    Packet_Guild_Upgrade_Guild_Agit() : PacketHeader(0x6e3, 0x1a){};
} __attribute__((packed));

#endif  // PACKET_GUILD_UPGRADE_GUILD_AGIT_H
