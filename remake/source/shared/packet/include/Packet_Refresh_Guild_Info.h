//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REFRESH_GUILD_INFO_H
#define PACKET_REFRESH_GUILD_INFO_H

#include "PacketHeader.h"

class Packet_Refresh_Guild_Info : public PacketHeader {
public:
    unsigned int m_charNo;     // +0xa
    unsigned int m_guildKey;   // +0xe
    Packet_Refresh_Guild_Info() : PacketHeader(0x1bbe, 0x12){};
} __attribute__((packed));

#endif  // PACKET_REFRESH_GUILD_INFO_H
