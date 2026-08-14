//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_GUILD_CREATE_H
#define PACKET_REQUEST_GUILD_CREATE_H

#include "PacketHeader.h"

class Packet_Request_Guild_Create : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    char m_guildName[0x17];     // +0xe
    char m_extra[0xc];          // +0x25
    Packet_Request_Guild_Create() : PacketHeader(0x43f, 0x32){};
} __attribute__((packed));

#endif  // PACKET_REQUEST_GUILD_CREATE_H
