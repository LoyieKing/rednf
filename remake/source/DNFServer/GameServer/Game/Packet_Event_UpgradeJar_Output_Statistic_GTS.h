// ORIG 弱内联 ctor W _ZN40Packet_Event_UpgradeJar_Output_Statistic_GTSC1Ev
//   @0x08694528 (sz 0x37)：
//   PacketHeader(0x106e, 0x12)
//   movl $0x0, 0xa(this)   → m_itemCode
//   movl $0x0, 0xe(this)   → m_itemCount
// 布局 0x12 = 0x0a + 4 + 4；CUser.cpp SendEventJarOutputItemStatistics 消费。
#ifndef DNF_GAME_PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H_
#define DNF_GAME_PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H_

#include "PacketHeader.h"

class Packet_Event_UpgradeJar_Output_Statistic_GTS : public PacketHeader
{
public:
    Packet_Event_UpgradeJar_Output_Statistic_GTS()
        : PacketHeader(0x106e, 0x12)
    {
        m_itemCode = 0;
        m_itemCount = 0;
    }
    int m_itemCode;              // +0x0a（CUser.cpp:9809）
    int m_itemCount;             // +0x0e（CUser.cpp:9810）
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Event_UpgradeJar_Output_Statistic_GTS, 0x12)
#endif  // DNF_GAME_PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H_
