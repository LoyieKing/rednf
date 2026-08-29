#ifndef GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA_H_
#define GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 布局按 ORIG ctor 0x822e54e 反汇编：
//   PACKET_HEADER ctor(this) → ROI_Category ctor(this+0x31) →
//   memset(this,0,0x3d) → setCategory(0)/setPacketID(2)/setSize(0x3d)。
// 成员 PACKET_HEADER@+0x00（packed 0x12）、ROI_Category@+0x31，sizeof 0x3d。
#include "PacketDesign.h"
#include "ROI_Category.h"

#pragma pack(push, 1)
class PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA
{
public:
    // ORIG 0x822e54e _ZN36PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GAC1Ev (size 0x00000075)
    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA();   // 定义 ValueCtors.cpp

    nsl::PACKET_HEADER m_header;   // +0x00
    char m_pad12[0x1f];            // +0x12..0x30
    ROI_Category m_roiCategory;    // +0x31
};  // sizeof 0x3d
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA_H_
