// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_DeathTower_Statistic_Playdata_Job：ORIG 导出弱内联 ctor W @0x0860ffe6
// （PacketHeader(0xc3d, 0x16)）。字段布局来自 Statics/Statistics.cpp
// WriteDeathTowerPlayDataJobStatistic 的 packed Wire 视图（+0xa 起）与
// SendDeathTowerPlayInfo（0x0860f45c）的写入序；sizeof == 0x16。
#ifndef DNF_GAME_PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H_
#define DNF_GAME_PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H_

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_DeathTower_Statistic_Playdata_Job : public PacketHeader
{
public:
    // ORIG W @0x0860ffe6：PacketHeader(0xc3d, 0x16)。弱内联（头内定义），
    // 与 ORIG C1/C2 弱符号一致。
    Packet_DeathTower_Statistic_Playdata_Job()
        : PacketHeader(static_cast<unsigned short>(0xc3d),
                       static_cast<unsigned short>(0x16))
    {
    }

    char m_deathTowerType;    // +0x0a
    unsigned short m_level;   // +0x0b（SendDeathTowerPlayInfo 写入成员 +0x14 低字节）
    unsigned int m_characJob; // +0x0d（成员 +0x15 低字节）
    char m_characGrow;        // +0x11（成员 +0x16）
    int m_field;              // +0x12（clearStage）
};                            // sizeof == 0x16
#pragma pack(pop)

#endif  // DNF_GAME_PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H_
