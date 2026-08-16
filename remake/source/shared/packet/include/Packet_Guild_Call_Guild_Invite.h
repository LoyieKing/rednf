//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CALL_GUILD_INVITE_H
#define PACKET_GUILD_CALL_GUILD_INVITE_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Call_Guild_Invite : public PacketHeader {
public:
    unsigned int m_caller;      // +0xa
    unsigned int m_guildKey;    // +0xe
    char m_name[0x1e];          // +0x12
    Packet_Guild_Call_Guild_Invite() : PacketHeader(0x434, 0x30)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

#endif  // PACKET_GUILD_CALL_GUILD_INVITE_H
