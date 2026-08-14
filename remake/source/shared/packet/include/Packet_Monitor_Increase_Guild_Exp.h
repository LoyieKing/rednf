//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_INCREASE_GUILD_EXP_H
#define PACKET_MONITOR_INCREASE_GUILD_EXP_H

#include "PacketHeader.h"

class Packet_Monitor_Increase_Guild_Exp : public PacketHeader {
public:
    unsigned int m_charNo;    // +0xa
    unsigned int m_guildKey;  // +0xe
    unsigned int m_addExp;    // +0x12
    char m_field_16;          // +0x16
    char m_field_17;          // +0x17
    Packet_Monitor_Increase_Guild_Exp(): PacketHeader(0x40e,0x1c) {};
} __attribute__((packed));


#endif //PACKET_MONITOR_INCREASE_GUILD_EXP_H
