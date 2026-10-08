#ifndef GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GP_H_
#define GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GP_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e94c
// _ZN32PCK_AUCTION_ASK_AVERAGE_PRICE_GPC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x1f)
// + setCategory(0x12) + setPacketID(0x1) + setSize(0x1f)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x1f（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

// [DNF-FIX 2026-10-08 c32] ORIG 0x08213b33/0x08213b41/0x08213b47（pkt -0x53：uid -0x41、charac -0x3d、
//   itemIdx -0x39）⇒ +0x12/+0x16/+0x1a、flag +0x1e，sizeof 0x1f。旧定义无 pack(1)，字段被
//   对齐到 +0x14/+0x18/+0x1c/+0x20，SendPacket(0x1f) 发出的字段全部错位、flag 被截掉（真 bug）。
#pragma pack(push, 1)
struct PCK_AUCTION_ASK_AVERAGE_PRICE_GP : public nsl::PACKET_HEADER
{
    unsigned int m_uid;         // +0x12
    int m_characNo;             // +0x16
    unsigned int m_itemIdx;     // +0x1a
    char m_flag;                // +0x1e

    PCK_AUCTION_ASK_AVERAGE_PRICE_GP()
    {
        PACKET_CTOR_BODY(0x12, 0x1, 0x1f);
    }
};
#pragma pack(pop)
static_assert(sizeof(PCK_AUCTION_ASK_AVERAGE_PRICE_GP) == 0x1f, "PCK_AUCTION_ASK_AVERAGE_PRICE_GP ORIG size 0x1f");

#endif  // GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GP_H_
