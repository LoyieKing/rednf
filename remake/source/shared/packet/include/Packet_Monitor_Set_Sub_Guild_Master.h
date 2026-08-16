//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SET_SUB_GUILD_MASTER_H
#define PACKET_MONITOR_SET_SUB_GUILD_MASTER_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Set_Sub_Guild_Master : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    char m_name[0x1e];          // +0x12
    unsigned char m_grade;      // +0x30
    Packet_Monitor_Set_Sub_Guild_Master() : PacketHeader(0x430, 0x31),
        m_charNo(0), m_guildKey(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));


#endif //PACKET_MONITOR_SET_SUB_GUILD_MASTER_H
