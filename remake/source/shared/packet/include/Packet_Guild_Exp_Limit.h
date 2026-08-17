//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_EXP_LIMIT_H
#define PACKET_GUILD_EXP_LIMIT_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Exp_Limit : public PacketHeader {
public:
    int m_channel;  // +a（SendGuildExpLimit：CEnvironment::get_channel_no）
    int m_guildExpLimit8;   // +e（SendGuildExpLimit：CDataManager GuildParameterScript+0xa0
                            //    = [guild levelup cost] 表 entry(0x14aa+8) 的 +4 dword（记录 second），
                            //    与 m_guildExpLimit[8] 同址重复读取；
                            //    guild 端 OnGuildExpLimit 为 stub（ORIG 0x80836e0 空函数），按表项语义命名）
    int m_guildExpLimit16;  // +12（SendGuildExpLimit：CDataManager GuildParameterScript+0xe0
                            //    = 表 entry(0x14aa+16) 的 +4 dword，与 m_guildExpLimit[16] 同址重复读取；
                            //    guild 端未消费，按表项语义命名）
    int m_guildExpLimit[17];  // +16..+59（SendGuildExpLimit 循环 i=0..16 读
                              //    CDataManager+0x1c+(0x14aa+i)*8 = GuildParameterScript+0x60+i*8，
                              //    即 [guild levelup cost] 8B 记录的 +4 dword。该值正是 game 侧
                              //    CUser::CheckGuildLevelUp（0x866c27e，经验不足回 0x54）与
                              //    CUser::increase_status（0x8666699，回 0xd2）经
                              //    GuildParameterScript::getGuildLevelUpParam(level)->+4 读取的
                              //    公会升级经验阈值，与 guild 侧 CGuildManager::m_ExpTable[17]
                              //    （默认 0x546/0x131e/0x2f76 = 官方表 2/3/4 级升级所需经验
                              //    1350/4894/12150）同构；接收端 OnGuildExpLimit 为 stub。
                              //    每记录 +0 dword（+0xa4/+0xe4 等）由 importGuildParameterScript
                              //    装载但 df_game_r 全二进制无读取点，无语义证据，未纳入包体）

    Packet_Guild_Exp_Limit() : PacketHeader(0x6e5, 0x5a),
        m_channel(0), m_guildExpLimit8(0), m_guildExpLimit16(0)
    {
    memset(m_guildExpLimit, 0, sizeof(m_guildExpLimit));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Guild_Exp_Limit, 0x5a);

#endif  // PACKET_GUILD_EXP_LIMIT_H
