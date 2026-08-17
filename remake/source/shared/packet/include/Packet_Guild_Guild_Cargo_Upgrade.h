//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_UPGRADE_H
#define PACKET_GUILD_GUILD_CARGO_UPGRADE_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Upgrade : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    int m_curCapacity;          // +0x12（Dispatcher_UpgradeGuildCargo: CUser::GetGuildCargoCapacity；
                                //          Guild OnGuildCargoUpgrade 与 GetCapacity() 比对）
    unsigned int m_newCapacity; // +0x16（旧容量 + 升级表 +0x10 增量；Guild SetCapacity；DBMW 写 cargo_capacity）
    unsigned int m_requiredFund;// +0x1a（GuildParameterScript 升级表 +4 所需资金；Guild SubGuildFund(reqFund)）
    Packet_Guild_Guild_Cargo_Upgrade() : PacketHeader(0x713, 0x1e),
        m_guildKey(0), m_charNo(0), m_curCapacity(0), m_newCapacity(0), m_requiredFund(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_GUILD_GUILD_CARGO_UPGRADE_H
