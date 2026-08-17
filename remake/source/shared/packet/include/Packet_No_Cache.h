//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NO_CACHE_H
#define PACKET_NO_CACHE_H

#include "PacketHeader.h"

class Packet_No_Cache : public PacketHeader {
public:
    unsigned int m_dbid;        // +0xa（第三轮穷举确认：guild 侧 OnNoticeGuildSecede/
                                //      OnNoticeGuildDismiss/OnGuildSecede 三处 noCache.ma = dbid；
                                //      monitor OnNoCache GetCacheCharacter(m_dbid)/Reset；
                                //      df_game_r Inter_GuildExpBookDelete@84e2f1e +0xa ←
                                //      inter.m_group(DBID)）
    unsigned int m_serverGroup; // +0xe（guild 侧 me = Get_ServerGroup()&0xff；
                                //      Inter_GuildExpBookDelete 转发并作 GetServerProxy 分组）
    unsigned int m_field12;     // +0x12（第三轮穷举 2026-08-16：guild 侧三处构造 m12 = 1/2
                                //      （1=SendAllTcpGameServer 广播、2=SendTcpGameServerFirst
                                //      首服模式）；monitor OnNoCache 未读取；game 侧发送
                                //      Inter_GuildExpBookDelete 不写 +0x12（ctor 默认 0）；
                                //      0x1b6d 游戏端接收反汇编不在语料，无接收侧语义证据，
                                //      保留占位）
    Packet_No_Cache() : PacketHeader(0x1b6d, 0x16),
        m_dbid(0), m_serverGroup(0), m_field12(0)
    {

    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_No_Cache, 0x16);

#endif  // PACKET_NO_CACHE_H
