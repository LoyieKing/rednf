// ORIG 弱内联 ctor W _ZN20Packet_MiniCraneSeedC2Ev @0x086d1d18 (sz 0x2d，C1/C2 同址)：
//   PacketHeader(0x27f8, 0xe)
//   movl $0x0, 0xa(this) → m_data
// 布局 0xe = 0x0a + 4；GameWorld.cpp UpdateMiniCraneSeed 整包 0xe 发送。
#ifndef DNF_GAME_PACKET_MINICRANESEED_H_
#define DNF_GAME_PACKET_MINICRANESEED_H_

#include "PacketHeader.h"

class Packet_MiniCraneSeed : public PacketHeader
{
public:
    Packet_MiniCraneSeed()
        : PacketHeader(0x27f8, 0xe)
    {
        m_data = 0;
    }
    unsigned int m_data;         // +0x0a（种子值，ORIG ctor 清零）
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_MiniCraneSeed, 0xe)
#endif  // DNF_GAME_PACKET_MINICRANESEED_H_
