//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_BUY_GUILD_SKILL_H
#define PACKET_MONITOR_BUY_GUILD_SKILL_H

#include "PacketHeader.h"

class Packet_Monitor_Buy_Guild_Skill : public PacketHeader {
public:
    unsigned int m_charNo;     // +0xa
    unsigned int m_guildKey;   // +0xe
    int m_skillId;             // +0x12
    int m_skillLevel;          // +0x16
    short m_slot;              // +0x1a
    unsigned int m_cost;       // +0x1c
    Packet_Monitor_Buy_Guild_Skill() : PacketHeader(0x429, 0x20){};
} __attribute__((packed));

#endif  // PACKET_MONITOR_BUY_GUILD_SKILL_H
