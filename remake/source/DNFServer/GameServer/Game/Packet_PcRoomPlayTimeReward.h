// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 复制定义（has_def=1，源 W8_EventClassifyB.cpp:76-88，inline ctor）
#ifndef DNF_GAME_PACKET_PCROOMPLAYTIMEREWARD_H_
#define DNF_GAME_PACKET_PCROOMPLAYTIMEREWARD_H_

#include "PacketHeader.h"

// ORIG W 0x08110c0a _ZN25Packet_PcRoomPlayTimeRewardC1Ev：
// PacketHeader(0x27ec,0x13) + byte@+0xa=0 + dword@+0xb=0 + dword@+0xf=0，sizeof == 0x13。
// 使用方：W8_EventClassifyB.cpp（引号 include 优先命中本目录定义；shared/packet/include
// 下的同名头为 monitor 等目标所用，成员名 m_serverGroup/m_accId/m_characNo，布局一致）。
class Packet_PcRoomPlayTimeReward : public PacketHeader
{
public:
    Packet_PcRoomPlayTimeReward()     // ORIG W 0x08110c0a
        : PacketHeader(static_cast<unsigned short>(0x27ec),
                       static_cast<unsigned short>(0x13))
    {
        m_field0a = 0;            // ORIG movb $0x0,0xa
        m_accId = 0;              // ORIG movl $0x0,0xb
        m_characNo = 0;           // ORIG movl $0x0,0xf
    }

    unsigned char m_field0a;          // +0x0a（ctor 清 0；调用方写 serverGroup）
    unsigned int  m_accId;            // +0x0b（ORIG movl $0x0,0xb —— 不对齐，须 packed）
    unsigned int  m_characNo;         // +0x0f（ORIG movl $0x0,0xf）
} __attribute__((packed));            // 基类 PacketHeader 已 packed（sizeof 0xa）；
                                      // 派生类不加 packed 会自然对齐到 +0xc/+0x10
                                      // （WR 审计 2026-08-30 实证 NEW 曾偏移 0xc/0x10）
TEST_CLASS_SIZE(Packet_PcRoomPlayTimeReward, 0x13);

#endif  // DNF_GAME_PACKET_PCROOMPLAYTIMEREWARD_H_
