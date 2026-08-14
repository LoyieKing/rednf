//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H
#define PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_Level_Up : public PacketHeader {
public:
    unsigned int m_padA;        // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned int m_guildKey;    // +0x12
    unsigned char m_pad16;      // +0x16
    unsigned int m_fund;        // +0x17
    Packet_Monitor_Call_Guild_Level_Up(): PacketHeader(0x408,0x1b) {};
} __attribute__((packed));


#endif //PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H
