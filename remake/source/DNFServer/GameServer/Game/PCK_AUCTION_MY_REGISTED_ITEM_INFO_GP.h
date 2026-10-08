#ifndef GAME_PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP_H_
#define GAME_PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP_H_

// ORIG W 0x0822eb70：PACKET_HEADER ctor + memset(this,0,0x1a)
// + setCategory(0x12) + setPacketID(8) + setSize(0x1a)。
#include "PacketDesign.h"

// [DNF-FIX 2026-10-08 c32] ORIG 0x08216a9c/0x08216aaa（pkt -0x3b：uid -0x29、charac -0x25） ⇒ m_uid@+0x12 / m_characNo@+0x16，
//   sizeof 0x1a = setSize。旧定义无 pack(1)，m_uid 被对齐到 +0x14、m_characNo 到 +0x18，
//   SendPacket 只发 0x1a 字节 ⇒ 拍卖服收到错位的 uid/角色号（真 bug）。
#pragma pack(push, 1)
struct PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP : public nsl::PACKET_HEADER
{
    unsigned int m_uid;       // +0x12
    unsigned int m_characNo;  // +0x16

    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP()
    {
        PACKET_CTOR_BODY(0x12, 0x8, 0x1a);
    }
};
#pragma pack(pop)
static_assert(sizeof(PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP) == 0x1a, "PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP ORIG size 0x1a");

#endif
