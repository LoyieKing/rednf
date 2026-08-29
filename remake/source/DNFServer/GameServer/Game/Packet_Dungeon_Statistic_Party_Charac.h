// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PartyClosureC2.cpp 局部定义
// Packet_Dungeon_Statistic_Party_Charac：ORIG 弱内联 ctor W @0x085bee2e；
// 定义源：Game/PartyClosureC2.cpp:182（PacketHeader(0xc39, 0x47) + packed 字段布局）。
#ifndef DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H_
#define DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H_

#include "PacketHeader.h"  // source/shared/packet/include/PacketHeader.h

class Packet_Dungeon_Statistic_Party_Charac : public PacketHeader
{
public:
    Packet_Dungeon_Statistic_Party_Charac();  // ORIG W @0x085bee2e
    unsigned short m_channel;        // +0x0a
    int m_dungeonIndex;              // +0x0c
    unsigned char m_dungeonDiff;     // +0x10
    unsigned char m_battleFlag354;   // +0x11（party+0x354）
    bool m_flag;                     // +0x12
    int m_job;                       // +0x13
    unsigned char m_growType;        // +0x17
    unsigned char m_collect;         // +0x18
    int m_playTime_f00;              // +0x19
    int m_f04;                       // +0x1d
    int m_f08;                       // +0x21
    int m_f0c;                       // +0x25
    int m_f10;                       // +0x29
    int m_f14;                       // +0x2d
    int m_f18;                       // +0x31
    int m_f1c;                       // +0x35
    int m_f34;                       // +0x39
    int m_f30;                       // +0x3d
    unsigned short m_level_f24;      // +0x41
    unsigned int m_memberCount;      // +0x43
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H_
