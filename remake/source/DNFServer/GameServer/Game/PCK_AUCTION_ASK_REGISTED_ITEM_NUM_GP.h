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

    // 字段化（guide §8.3）：pad 拆具名字段，偏移/sizeof 与原骨架完全一致。
    // 证据：ORIG dispatch_sig 0x08213e8a cera 分支 +0x1af..+0x50f 逐字段访问
    //   +0x12 char_idx(GetUID)、+0x16 character_id、+0x1a get_byte(uchar&)、
    //   +0x1b get_short(ushort&)、+0x1d item_id（uint 临时转存）、+0x21/25/29 get_int(int&)；
    //   GP 无 unit_price / ROI_Category（sizeof 0x2d）。字段名同 GA。
    nsl::PACKET_HEADER m_header;   // +0x00
    int char_idx;                  // +0x12
    int character_id;              // +0x16
    unsigned char inven_type;      // +0x1a
    unsigned short slot_no;        // +0x1b
    unsigned long item_id;         // +0x1d
    int add_info;                  // +0x21
    int price;                     // +0x25
    int instant_price;             // +0x29
};  // sizeof 0x2d
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP_H_
