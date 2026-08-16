//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_INFO_H
#define PACKET_NOTICE_GUILD_WAR_INFO_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_Info : public PacketHeader {
public:
    unsigned int m_padA;         // +0xa
    unsigned int m_charNo;       // +0xe
    char m_rankInfo[0x14a];      // +0x12
    Packet_Notice_Guild_War_Info() : PacketHeader(0x414, 0x15c){};
} __attribute__((packed));

#endif  // PACKET_NOTICE_GUILD_WAR_INFO_H
