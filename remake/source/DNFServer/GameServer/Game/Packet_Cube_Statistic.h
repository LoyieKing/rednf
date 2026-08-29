// ORIG 弱内联 ctor W _ZN21Packet_Cube_StatisticC1Ev @0x0861002e (sz 0x50)：
//   PacketHeader(0x0c33, 0x17)
//   movw $0x0, 0xa(this)   → m_channel
//   movw $0x0, 0xc(this)   → m_level
//   movl $0x0, 0xe(this)   → m_itemIndex
//   movl $0x0, 0x12(this)  → m_itemCount
//   movb $0x0, 0x16(this)  → m_type
// 布局 0x17 = 0x0a + 2+2+4+4+1；CCubeStatistic.cpp 消费（ushort/ushort/uint/int/uchar）。
#ifndef DNF_GAME_PACKET_CUBE_STATISTIC_H_
#define DNF_GAME_PACKET_CUBE_STATISTIC_H_

#include "PacketHeader.h"

class Packet_Cube_Statistic : public PacketHeader
{
public:
    Packet_Cube_Statistic()
        : PacketHeader(0x0c33, 0x17)
    {
        m_channel = 0;
        m_level = 0;
        m_itemIndex = 0;
        m_itemCount = 0;
        m_type = 0;
    }
    unsigned short m_channel;    // +0x0a
    unsigned short m_level;      // +0x0c
    unsigned int m_itemIndex;    // +0x0e
    int m_itemCount;             // +0x12
    unsigned char m_type;        // +0x16
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Cube_Statistic, 0x17)
#endif  // DNF_GAME_PACKET_CUBE_STATISTIC_H_
