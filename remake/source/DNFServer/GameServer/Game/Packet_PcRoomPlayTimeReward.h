// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 复制定义（has_def=1，源 W8_EventClassifyB.cpp:76-88，inline ctor）
#ifndef DNF_GAME_PACKET_PCROOMPLAYTIMEREWARD_H_
#define DNF_GAME_PACKET_PCROOMPLAYTIMEREWARD_H_

#include "PacketHeader.h"

// ORIG W 0x08110c0a _ZN25Packet_PcRoomPlayTimeRewardC1Ev：
// PacketHeader(0x27ec,0x13) + byte@+0xa=0 + dword@+0xb=0 + dword@+0xf=0，sizeof == 0x13。
// 注意：W8_EventClassifyB.cpp:76 已有同构 inline 定义（ctor 内联于该 TU），
// 未删除 —— 引入本头的 TU 需避免重复定义；cpp 侧清理由宿主统一安排（冲突已记录）。
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
    unsigned int  m_accId;            // +0x0b
    unsigned int  m_characNo;         // +0x0f
};                                    // sizeof == 0x13（源定义注释，未加断言）

#endif  // DNF_GAME_PACKET_PCROOMPLAYTIMEREWARD_H_
