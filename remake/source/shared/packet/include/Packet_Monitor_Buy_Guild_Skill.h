//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_BUY_GUILD_SKILL_H
#define PACKET_MONITOR_BUY_GUILD_SKILL_H

#include "PacketHeader.h"

class Packet_Monitor_Buy_Guild_Skill : public PacketHeader {
public:
    Packet_Monitor_Buy_Guild_Skill() : PacketHeader(0x429, 0x20){};
};

#endif  // PACKET_MONITOR_BUY_GUILD_SKILL_H
