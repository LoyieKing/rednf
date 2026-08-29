// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_36 仅 ctor）+ 定义提取自 Game/PartyClosureC2.cpp:163-180。
// packed 布局，ctor PacketHeader(0xc37, 0x1f) 外联在 cpp，接入时由宿主迁除。
#ifndef DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_JOB_H_
#define DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_JOB_H_

#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party_Job : public PacketHeader
{
public:
    Packet_Dungeon_Statistic_Party_Job();  // ORIG W 0x085bee0a
    unsigned short m_channel;        // +0x0a
    int m_dungeonIndex;              // +0x0c
    unsigned char m_dungeonDiff;     // +0x10
    unsigned char m_battleFlag355;   // +0x11
    unsigned char m_battleFlag356;   // +0x12
    unsigned char m_battleFlag357;   // +0x13
    bool m_flag;                     // +0x14
    unsigned char m_collect;         // +0x15
    int m_job;                       // +0x16
    unsigned char m_growType;        // +0x1a
    unsigned int m_memberCount;      // +0x1b
} __attribute__((packed));           // sizeof 0x1f

#endif  // DNF_GAME_PACKET_DUNGEON_STATISTIC_PARTY_JOB_H_
