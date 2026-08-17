//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_UPGRADE_GUILD_AGIT_H
#define PACKET_GUILD_UPGRADE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Upgrade_Guild_Agit : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    union {
        unsigned int m_powerWarPoint;  // +0x12（SendUpgradeGuildAgit param_3；接收端 OnUpgradeGuildAgit 传入
                                       //      CGuild::UpgradeGuildAgit(c)，函数内 SubPowerWarPoint(c)——升级公会根据地消耗的决斗点）
        unsigned int m_field12;        // +0x12（旧占位名，guild 端 OnUpgradeGuildAgit 引用）
    };
    unsigned int m_fund;        // +0x16
    Packet_Guild_Upgrade_Guild_Agit() : PacketHeader(0x6e3, 0x1a),
        m_guildKey(0), m_charNo(0), m_powerWarPoint(0), m_fund(0)
    {

    };
} __attribute__((packed));

#endif  // PACKET_GUILD_UPGRADE_GUILD_AGIT_H
