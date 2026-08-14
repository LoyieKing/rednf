//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_INFO_H
#define PACKET_MONITOR_CALL_GUILD_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_Info : public PacketHeader {
public:
    unsigned int m_fieldA;       // +0xa
    unsigned int m_charNo;       // +0xe
    unsigned char m_count;       // +0x12
    unsigned int m_guildKeys[1]; // +0x13
    Packet_Monitor_Call_Guild_Info() : PacketHeader(0x40d, 0x18f){};
} __attribute__((packed));

#endif  // PACKET_MONITOR_CALL_GUILD_INFO_H
