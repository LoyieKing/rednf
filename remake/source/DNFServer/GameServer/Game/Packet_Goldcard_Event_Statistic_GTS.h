// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_52 仅 ctor）+ 定义提取自 Game/CPartyDepsD2.cpp:1466-1480。
// ORIG ctor 0x085bee96 W：PacketHeader(0x1f46,0x13)[10] + byte + int + int（总 0x13）。
// 接入时由宿主迁除 cpp 内重复段。
#ifndef DNF_GAME_PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H_
#define DNF_GAME_PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H_

#include "PacketHeader.h"

struct Packet_Goldcard_Event_Statistic_GTS
{
    PacketHeader m_header;   // +0x00（packed 0xa）
    unsigned char m_fieldA;  // +0x0a（活动类型）
    int m_fieldB;            // +0x0b
    int m_fieldF;            // +0x0f

    Packet_Goldcard_Event_Statistic_GTS()
        : m_header(0x1f46, 0x13)
    {
        m_fieldA = 0;
        m_fieldB = 0;
        m_fieldF = 0;
    }
} __attribute__((packed));   // sizeof == 0x13

#endif  // DNF_GAME_PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H_
