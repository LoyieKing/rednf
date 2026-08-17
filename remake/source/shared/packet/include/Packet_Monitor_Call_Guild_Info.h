//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_INFO_H
#define PACKET_MONITOR_CALL_GUILD_INFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Call_Guild_Info : public PacketHeader {
public:
    unsigned int m_uid;          // +0xa  SendCallGuildInfo param_2 = CUser::GetUID（Dispatcher_CallGuildInfo）
    unsigned int m_charNo;       // +0xe
    unsigned char m_count;       // +0x12
    unsigned int m_guildKeys[95]; // +0x13..+0x18e（count < 0x60，size=count*4+0x13，max 0x18f）
    Packet_Monitor_Call_Guild_Info() : PacketHeader(0x40d, 0x18f),
        m_uid(0), m_charNo(0), m_count(0)
    {
    memset((char*)this + 0x13, 0, 0x17c);
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Call_Guild_Info, 0x18f);

#endif  // PACKET_MONITOR_CALL_GUILD_INFO_H
