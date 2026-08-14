//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_GUILD_CHAT_H
#define PACKET_MONITOR_GUILD_CHAT_H

#include "PacketHeader.h"

class Packet_Monitor_Guild_Chat : public PacketHeader {
public:
    unsigned int m_charNo;    // +0xa
    unsigned int m_guildKey;  // +0xe
    unsigned char m_msgLen;   // +0x12
    char m_msg[0x100];        // +0x13
    Packet_Monitor_Guild_Chat() : PacketHeader(0x400, 0x113){};
} __attribute__((packed));

#endif  // PACKET_MONITOR_GUILD_CHAT_H
