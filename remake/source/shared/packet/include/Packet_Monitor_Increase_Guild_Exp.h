//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_INCREASE_GUILD_EXP_H
#define PACKET_MONITOR_INCREASE_GUILD_EXP_H

#include "PacketHeader.h"

class Packet_Monitor_Increase_Guild_Exp : public PacketHeader {
public:
    unsigned int m_charNo;    // +0xa（WorkPerFiveMin：getCurCharacNo；SendIncreaseGuildExp param_2）
    unsigned int m_guildKey;  // +0xe（WorkPerFiveMin：get_charac_guildkey；SendIncreaseGuildExp param_1）
    unsigned int m_addExp;    // +0x12（WorkPerFiveMin：get_guild_exp / get_guild_today_exp；SendIncreaseGuildExp param_3）
    char m_notify;            // +0x16（Guild 接收端：非 0 → SendGuildInfoToMembers；SendIncreaseGuildExp param_4；WorkPerFiveMin 不写）
    char m_expType;           // +0x17（0=一般经验 / 1=SendIncreaseGuildExp / 2=今日经验；接收端按 0/1/2 选经验上限 max1/max2）
    Packet_Monitor_Increase_Guild_Exp() : PacketHeader(0x40e, 0x1c),
        m_expType(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_MONITOR_INCREASE_GUILD_EXP_H
