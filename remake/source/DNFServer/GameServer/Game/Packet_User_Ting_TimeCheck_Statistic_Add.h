// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
#ifndef DNF_GAME_PACKET_USER_TING_TIMECHECK_STATISTIC_ADD_H_
#define DNF_GAME_PACKET_USER_TING_TIMECHECK_STATISTIC_ADD_H_

// [DNF-FIX 2026-10-08 c33] ORIG W 0x08694504 = PacketHeader(0xc45, 0x12)；CUser::DisConnSig ting@-0xaa，
//   0x08648ef2 写 -0xa0（+0xa 在线秒数）、0x08648f01 写 -0x9c（+0xe m_accId），0x08648f0d/f17
//   回读 +0xa 判区间，SendPacket(&ting,0x12) ⇒ 0x12 字节包 = 头 + 两个 4 字节字段。
//   旧空壳（sizeof 1）+ OnlyOrigBatch5 引入的 shared 版 ctor 写 10 字节头 ⇒ 越界写，
//   且发出的 +0xa..+0x11 是无关栈字节而非 playSec/accId（真 bug）。
#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_User_Ting_TimeCheck_Statistic_Add : public PacketHeader
{
public:
    Packet_User_Ting_TimeCheck_Statistic_Add() : PacketHeader(0xc45, 0x12) {}   // W 0x08694504
    int m_playSec;            // +0x0a（getCurSec - GetLoginTime）
    unsigned int m_accId;     // +0x0e（CUser::m_accId 直读）
};
#pragma pack(pop)
static_assert(sizeof(Packet_User_Ting_TimeCheck_Statistic_Add) == 0x12, "Packet_User_Ting_TimeCheck_Statistic_Add ORIG size 0x12");

#endif  // DNF_GAME_PACKET_USER_TING_TIMECHECK_STATISTIC_ADD_H_
