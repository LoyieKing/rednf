//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CARGO_REQUST_H
#define PACKET_GUILD_CARGO_REQUST_H

#include "PacketHeader.h"

class Packet_Guild_Cargo_Requst : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    Packet_Guild_Cargo_Requst() : PacketHeader(0x70a, 0x12){};
} __attribute__((packed));

#endif  // PACKET_GUILD_CARGO_REQUST_H
