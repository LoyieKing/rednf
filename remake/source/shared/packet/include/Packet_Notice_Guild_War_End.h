//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_END_H
#define PACKET_NOTICE_GUILD_WAR_END_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_End : public PacketHeader {
public:
    unsigned char m_fieldA;  // +0xa
    Packet_Notice_Guild_War_End() : PacketHeader(0x412, 0xb){};
} __attribute__((packed));

#endif  // PACKET_NOTICE_GUILD_WAR_END_H
