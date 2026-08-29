// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_DeathTower_Statistic_Playdata_Party：ORIG 导出弱内联 ctor W @0x0861000a
// （PacketHeader(0xc3f, 0x10)）。字段布局来自 Statics/Statistics.cpp
// WriteDeathTowerPlayDataPartyStatistic 的 packed Wire 视图（+0xa 起）与
// SendDeathTowerPlayInfo（0x0860f45c）的写入序；sizeof == 0x10。
#ifndef DNF_GAME_PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H_
#define DNF_GAME_PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H_

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_DeathTower_Statistic_Playdata_Party : public PacketHeader
{
public:
    // ORIG W @0x0861000a：PacketHeader(0xc3f, 0x10)。弱内联（头内定义），
    // 与 ORIG C1/C2 弱符号一致。
    Packet_DeathTower_Statistic_Playdata_Party()
        : PacketHeader(static_cast<unsigned short>(0xc3f),
                       static_cast<unsigned short>(0x10))
    {
    }

    char m_deathTowerType;    // +0x0a
    char m_partyCount;        // +0x0b（SendDeathTowerPlayInfo 写入 param_5 低字节）
    int m_field;              // +0x0c（clearStage）
};                            // sizeof == 0x10
#pragma pack(pop)

#endif  // DNF_GAME_PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H_
