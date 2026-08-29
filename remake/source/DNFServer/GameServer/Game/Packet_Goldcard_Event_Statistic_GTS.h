// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_52 仅 ctor）+ 定义提取自 Game/CPartyDepsD2.cpp:1466-1480。
// ORIG ctor 0x085bee96 W：PacketHeader(0x1f46,0x13) + GoldCardEventStatisticC1(this+0xa)，
// +0xa 为 GoldCardEventStatistic 子对象（size 0x9，总 0x13）。
#ifndef DNF_GAME_PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H_
#define DNF_GAME_PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H_

#include "PacketHeader.h"
#include "GoldCardEventStatistic.h"

struct Packet_Goldcard_Event_Statistic_GTS
{
    PacketHeader m_header;          // +0x00（packed 0xa）
    GoldCardEventStatistic m_stat;  // +0x0a（ORIG GoldCardEventStatisticC1 构造）

    Packet_Goldcard_Event_Statistic_GTS()
        : m_header(0x1f46, 0x13), m_stat()
    {
    }
} __attribute__((packed));   // sizeof == 0x13

#endif  // DNF_GAME_PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H_
