//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_INFO_H
#define PACKET_NOTICE_GUILD_WAR_INFO_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_Info : public PacketHeader {
public:
    unsigned int m_uid;          // +0xa（dispatch_sig：CUser::GetUID）
    unsigned int m_characNo;     // +0xe（dispatch_sig：CUser::get_charac_no）
    char m_rankInfo[0x14a];      // +0x12
    // ORIG C1 W 0x0822badc：PacketHeader(0x414,0x15c) + 10 个 ST_Guild_War_Rank_Info
    //（+0x12 起 stride 0x21；esi 由 9 计到 -1＝10 次，填满 0x12..0x15c）。
    // ctor 定义在 Game/DisPatcherCore.cpp（ST_Guild_War_Rank_Info 完整布局在 Game 域）。
    Packet_Notice_Guild_War_Info();
} __attribute__((packed));

#endif  // PACKET_NOTICE_GUILD_WAR_INFO_H
