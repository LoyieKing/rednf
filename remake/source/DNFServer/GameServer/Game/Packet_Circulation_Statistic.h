// ORIG 弱内联 ctor W _ZN26Packet_Circulation_StatisticC1Ev @0x086100f8 (sz 0x39)：
//   PacketHeader(0x17cb, 0xd0)
//   movl $0x0, 0xa(this)   → 清零 m_channel+m_level（+0x0a..0x0d）
//   movw $0x0, 0xce(this)  → 清零 +0xce ushort
// 布局（0xd0 = 0x0a + 2 + 2 + 0xc0 + 2）：CCirculationStatistic.cpp memcpy
//   m_data 0xc0 字节，故 m_data 起点 +0x0e，尾部 +0xce ushort。
#ifndef DNF_GAME_PACKET_CIRCULATION_STATISTIC_H_
#define DNF_GAME_PACKET_CIRCULATION_STATISTIC_H_

#include "PacketHeader.h"

class Packet_Circulation_Statistic : public PacketHeader
{
public:
    Packet_Circulation_Statistic()
        : PacketHeader(0x17cb, 0xd0)
    {
        // [b22] ORIG ctor @0x086100f8 用单条 movl $0,0xa(this) 连同
        // +0x0a..0x0d 一次清零——两独立 ushort 赋值会发两条 movw → DIFF。
        // 无业务消费者单独读 m_channel/m_level，合并为 4 字节零清。
        m_channelLevel = 0;
        m_fieldCE = 0;
    }
    unsigned int m_channelLevel; // +0x0a（= m_channel@+0x0a + m_level@+0x0c，ORIG 单 movl 清零）
    char m_data[0xc0];           // +0x0e（0x30 * sizeof(int) 统计值）
    unsigned short m_fieldCE;    // +0xce（ORIG ctor movw $0x0）
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Circulation_Statistic, 0xd0)
#endif  // DNF_GAME_PACKET_CIRCULATION_STATISTIC_H_
