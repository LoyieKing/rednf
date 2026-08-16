//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CHANGE_GUILD_NAME_H
#define PACKET_CHANGE_GUILD_NAME_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Change_Guild_Name : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    char m_name[0x1e];          // +0x12
    Packet_Change_Guild_Name() : PacketHeader(0x444, 0x29),
        m_guildKey(0), m_charNo(0)
    {
    memset((char*)this + 0x12, 0, 0x17);  // m_name
    };
} __attribute__((packed));

#endif  // PACKET_CHANGE_GUILD_NAME_H
