//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SEND_GUILD_MAIL_H
#define PACKET_MONITOR_SEND_GUILD_MAIL_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Send_Guild_Mail : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildId;     // +0xe
    char m_msg[0x100];          // +0x12
    Packet_Monitor_Send_Guild_Mail() : PacketHeader(0x432, 0x112)
    {
    memset(m_msg, 0, sizeof(m_msg));
    };
} __attribute__((packed));

#endif  // PACKET_MONITOR_SEND_GUILD_MAIL_H
