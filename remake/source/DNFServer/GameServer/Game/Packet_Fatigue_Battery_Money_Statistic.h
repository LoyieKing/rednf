#ifndef DNF_GAME_PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H_
#define DNF_GAME_PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CFatigueBatteryHandle.cpp@28（源码定义复制）
// ORIG W ctor _ZN38Packet_Fatigue_Battery_Money_StatisticC2Ev 0x084a84ee。

#include "PacketHeader.h"

// 类声明与布局复制自 CFatigueBatteryHandle.cpp:28-39
//（ctor 语义 PacketHeader(0xc4a,0x11)；byte+0xa=0、int+0xb=0、short+0xf=0）。
class Packet_Fatigue_Battery_Money_Statistic : public PacketHeader
{
public:
    Packet_Fatigue_Battery_Money_Statistic()
        : PacketHeader(0xc4a, 0x11), m_serverGroup(0), m_fieldB(0), m_grownupBuff(0)
    {
    }

    unsigned char m_serverGroup;   // +0xa
    unsigned int m_fieldB;         // +0xb
    unsigned short m_grownupBuff;  // +0xf
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H_
