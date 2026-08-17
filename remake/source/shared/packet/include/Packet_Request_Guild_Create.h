//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_GUILD_CREATE_H
#define PACKET_REQUEST_GUILD_CREATE_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Request_Guild_Create : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    char m_guildName[0x17];     // +0xe
    char m_extra[0xc];          // +0x25
    Packet_Request_Guild_Create() : PacketHeader(0x43f, 0x32)
    {
    // 2026-08-16 修正：ORIG ctor（0x846fa60）先 memset +0x25(0xd) 再 memset
    // m_guildName(0x17)
    memset((char*)this + 0x25, 0, 0xd);
    memset(m_guildName, 0, sizeof(m_guildName));
    };
} __attribute__((packed));

#endif  // PACKET_REQUEST_GUILD_CREATE_H
