#ifndef GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP_H_
#define GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP_H_

// 布局按 ORIG ctor 0x822e9b4 反汇编：
//   PACKET_HEADER ctor(this) → memset(this,0,0x2d) →
//   setCategory(0x12)/setPacketID(2)/setSize(0x2d)。
// 仅 PACKET_HEADER 成员（无 ROI_Category），sizeof 0x2d。
#include "PacketDesign.h"

#pragma pack(push, 1)
class PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP
{
public:
    // ORIG 0x822e9b4 _ZN36PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GPC1Ev (size 0x00000067)
    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP();   // 定义 ValueCtors.cpp

    nsl::PACKET_HEADER m_header;   // +0x00
    char m_pad12[0x1b];            // +0x12..0x2c
};  // sizeof 0x2d
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP_H_
