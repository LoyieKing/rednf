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

    // 字段化（guide §8.3）：pad 拆具名字段，偏移/sizeof 与原骨架完全一致。
    // 证据：ORIG dispatch_sig 0x08213e8a（Dispatcher_AuctionRegistItem）逐字段访问
    //   +0x12 char_idx(GetUID)、+0x16 character_id、+0x1a get_byte(uchar&)、
    //   +0x1b get_short(ushort&)、+0x1d item_id（uint 临时转存）、+0x21/25/29/2d get_int(int&)、
    //   +0x31 ROI_Category（_sort）。字段名同 ServerLab
    //   auction_header/AuctionPacket.h:135 PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA。
    nsl::PACKET_HEADER m_header;   // +0x00
    int char_idx;                  // +0x12
    int character_id;              // +0x16
    unsigned char inven_type;      // +0x1a
    unsigned short slot_no;        // +0x1b
    unsigned long item_id;         // +0x1d
    int add_info;                  // +0x21
    int price;                     // +0x25
    int instant_price;             // +0x29
    int unit_price;                // +0x2d
    ROI_Category m_roiCategory;    // +0x31
};  // sizeof 0x3d
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA_H_
