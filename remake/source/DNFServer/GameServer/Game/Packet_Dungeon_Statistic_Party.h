// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PartyClosureC2.cpp
// Packet_Dungeon_Statistic_Party：ORIG 弱内联 ctor W @0x085bede6。
// has_def=1：完整复制定义自 PartyClosureC2.cpp:136（class : PacketHeader，
// PacketHeader(0xc35,0x40)，字段偏移逐条对应报告汇编；同族 Job 0x085bee0a /
// Charac 0x085bee2e / Blood 0x085bee52 留在原 TU）。ctor 体仍由 PartyClosureC2.cpp 提供。
#ifndef DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_H_
#define DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_H_

#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party : public PacketHeader
{
public:
    Packet_Dungeon_Statistic_Party();   // ORIG W @0x085bede6（体：PacketHeader(0xc35,0x40)，PartyClosureC2.cpp）
    unsigned short m_channel;        // +0x0a
    int m_dungeonIndex;              // +0x0c
    unsigned char m_dungeonDiff;     // +0x10
    unsigned char m_battleFlag355;   // +0x11（party+0x355）
    unsigned char m_battleFlag356;   // +0x12
    unsigned char m_battleFlag357;   // +0x13
    bool m_flag;                     // +0x14
    unsigned char m_collect;         // +0x15（user+0x8e054）
    int m_playTime_f00;              // +0x16
    int m_f04;                       // +0x1a
    int m_f08;                       // +0x1e
    int m_f0c;                       // +0x22
    int m_f10;                       // +0x26
    int m_f14;                       // +0x2a
    int m_f18;                       // +0x2e
    int m_f1c;                       // +0x32
    int m_f34;                       // +0x36
    int m_f30;                       // +0x3a
    unsigned short m_level_f24;      // +0x3e
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_H_
