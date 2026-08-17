//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CREATE_GUILD_AGIT_H
#define PACKET_GUILD_CREATE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Create_Guild_Agit : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa（SendCreateGuildAgit param_1；接收端 FindGuild）
    unsigned int m_charNo;      // +0xe（SendCreateGuildAgit param_2；接收端 IsGuildMaster）
    union {
        unsigned int m_powerWarPoint;  // +0x12（SendCreateGuildAgit param_3；接收端 OnCreateGuildAgit 传入
                                       //      CGuild::CreateGuildAgit(c)，函数内 SubPowerWarPoint(c)——建公会根据地消耗的决斗点）
        unsigned int m_field12;        // +0x12（旧占位名，guild 端 OnCreateGuildAgit 引用）
    };
    unsigned int m_fund;        // +0x16（SendCreateGuildAgit param_4；接收端校验 guild fund）
    Packet_Guild_Create_Guild_Agit() : PacketHeader(0x6dc, 0x1a),
        m_guildKey(0), m_charNo(0), m_powerWarPoint(0), m_fund(0)
    {

    };
} __attribute__((packed));

#endif  // PACKET_GUILD_CREATE_GUILD_AGIT_H
