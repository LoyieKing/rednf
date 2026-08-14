//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_APPROVE_JOIN_GUILD_H
#define PACKET_APPROVE_JOIN_GUILD_H

#include "PacketHeader.h"

class Packet_Approve_Join_Guild : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    unsigned int m_no;        // +0x12
    Packet_Approve_Join_Guild() : PacketHeader(0x1bc2, 0x16){};
} __attribute__((packed));

#endif  // PACKET_APPROVE_JOIN_GUILD_H
