//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_INFO_H
#define PACKET_MONITOR_CALL_GUILD_INFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Call_Guild_Info : public PacketHeader {
public:
    unsigned int m_fieldA;       // +0xa
    unsigned int m_charNo;       // +0xe
    unsigned char m_count;       // +0x12
    unsigned int m_guildKeys[1]; // +0x13
    Packet_Monitor_Call_Guild_Info() : PacketHeader(0x40d, 0x18f),
        m_fieldA(0), m_charNo(0), m_count(0)
    {
    memset((char*)this + 0x13, 0, 0x17c);
    };
} __attribute__((packed));

#endif  // PACKET_MONITOR_CALL_GUILD_INFO_H
