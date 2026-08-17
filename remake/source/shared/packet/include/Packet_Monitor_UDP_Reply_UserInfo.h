//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_REPLY_USERINFO_H
#define PACKET_MONITOR_UDP_REPLY_USERINFO_H

#include "PacketHeader.h"
#include <string.h>

struct ST_MonitorReplyUserInfo
{
    unsigned int m_dbid;        // +0x0  FindUser / CreateUser dbid（get_acc_id）
    unsigned int m_charNo;      // +0x4  get_charac_no
    int m_channel;              // +0x8  CreateUser channel / idByChannel（game 发送端写 CUser::GetUID）
#ifdef DNF_SVC_GUILD
    int m_memberKey;            // +0xc  GuildMemLogin member key
#else
    int m_guildKey;             // +0xc  Send_userinfos_to_upper_server: get_charac_guildkey
#endif
    char m_job;                 // +0x10
    char m_grade;               // +0x11 getCurCharacGrowType
    short m_level;              // +0x12 get_charac_level
    char m_name[0x1e];          // +0x14 getCurCharacName strcpy
#ifdef DNF_SVC_GUILD
    char m_pad32[4];            // +0x32（第三轮穷举 2026-08-16：df_game_r
                                //        Send_userinfos_to_upper_server@82961be（非 GUILD 构建）
                                //        写 get_charac_memberkey→+0x32、+0xc=guildkey；
                                //        Guild/Monitor OnReplyUserInfo 均用 +0xc m_memberKey
                                //        （GuildMemLogin），不读 +0x32；guild 构建下 +0x32
                                //        无语义消费，本分支保留）
#else
    int m_memberKey;            // +0x32 get_charac_memberkey
#endif
    char m_ip[0x10];            // +0x36 GetPeerIP2 + strcpy
    unsigned char m_sex;        // +0x46 getSex
    char m_ssn[7];              // +0x47 GetSsnString memcpy 6
} __attribute__((packed));

class Packet_Monitor_UDP_Reply_UserInfo : public PacketHeader {
public:
    unsigned char m_count;                      // +0xa
    unsigned char m_channel;                    // +0xb
    ST_MonitorReplyUserInfo m_items[75];        // +0xc
    Packet_Monitor_UDP_Reply_UserInfo() : PacketHeader(0x3ea, 0x16e6)
    {
    *(char*)((char*)this + 0xa) = 0;
    *(char*)((char*)this + 0xb) = 255;
    memset((char*)this + 0xc, 0, 0x16da);
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Reply_UserInfo, 0x16e6);

#endif //PACKET_MONITOR_UDP_REPLY_USERINFO_H
