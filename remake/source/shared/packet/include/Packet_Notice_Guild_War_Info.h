//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_INFO_H
#define PACKET_NOTICE_GUILD_WAR_INFO_H

#include "PacketHeader.h"
// Game 域头（元素类型权威声明点）；本头位于 shared/packet/include，-I 不含 Game 目录，
// 故用相对路径（与 Game/Packet_Notice_Guild_War_Info.h 的 ../../../shared/... 同法）。
#include "../../../DNFServer/GameServer/Game/ST_Guild_War_Rank_Info.h"

class Packet_Notice_Guild_War_Info : public PacketHeader {
public:
    unsigned int m_uid;          // +0xa（dispatch_sig：CUser::GetUID）
    unsigned int m_characNo;     // +0xe（dispatch_sig：CUser::get_charac_no）
    // +0x12..+0x15b：ORIG C1 由编译器隐式构造该数组（倒计数 esi=9→-1 共 10 次，
    // stride 0x21 = sizeof(ST_Guild_War_Rank_Info)）——声明为真实元素类型数组才能
    // 复现 P151 循环；手写 placement new 会多出 _ZnwjPv/size 装载 2 条（N83）。
    ST_Guild_War_Rank_Info m_rankInfo[10];
    // ORIG C1 W 0x0822badc：PacketHeader(0x414,0x15c) + 10 个 ST_Guild_War_Rank_Info
    //（+0x12 起 stride 0x21；esi 由 9 计到 -1＝10 次，填满 0x12..0x15c）。
    // ctor 定义在 Game/DisPatcherCore.cpp（ST_Guild_War_Rank_Info 完整布局在 Game 域）。
    Packet_Notice_Guild_War_Info();
} __attribute__((packed));

static_assert(sizeof(Packet_Notice_Guild_War_Info) == 0x15c,
              "Packet_Notice_Guild_War_Info ABI drift (ORIG C1 0x0822badc)");

#endif  // PACKET_NOTICE_GUILD_WAR_INFO_H
