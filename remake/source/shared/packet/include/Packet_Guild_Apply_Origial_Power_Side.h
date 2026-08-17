//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_APPLY_ORIGIAL_POWER_SIDE_H
#define PACKET_GUILD_APPLY_ORIGIAL_POWER_SIDE_H

#include "PacketHeader.h"

class Packet_Guild_Apply_Origial_Power_Side : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    union {
        unsigned char m_powerSide;  // +0x12（df_guild_r OnGuildApplyOriginalPowerSide: side==3||side==4，SetPowerSide；日志 PacketPowerSide）
        unsigned char m_field12;    // +0x12（旧占位名，df_guild_r OnGuildApplyOriginalPowerSide 引用）
    };
    Packet_Guild_Apply_Origial_Power_Side() : PacketHeader(0x6b2, 0x13),
        m_charNo(0), m_guildKey(0), m_powerSide(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_GUILD_APPLY_ORIGIAL_POWER_SIDE_H
