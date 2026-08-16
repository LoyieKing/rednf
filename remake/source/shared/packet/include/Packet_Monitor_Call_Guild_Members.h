//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_MEMBERS_H
#define PACKET_MONITOR_CALL_GUILD_MEMBERS_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_Members : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    Packet_Monitor_Call_Guild_Members() : PacketHeader(0x401, 0x12),
        m_guildKey(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_MONITOR_CALL_GUILD_MEMBERS_H
