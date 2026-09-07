#ifndef GAME_PCK_AUCTION_SEARCH_BY_ITEMKEY_GP_H_
#define GAME_PCK_AUCTION_SEARCH_BY_ITEMKEY_GP_H_

// 布局按 ORIG ctor 0x822ea84 反汇编：
//   PACKET_HEADER ctor(this) → TSearchByItemId_ ctor(this+0x1a) →
//   memset(this,0,0x81) → setCategory(0x12)/setPacketID(6)/setSize(0x81)。
// 成员 PACKET_HEADER@+0x00、TSearchByItemId_@+0x1a，sizeof 0x81。
#include "PacketDesign.h"
#include "TSearchByItemId_.h"

#pragma pack(push, 1)
class PCK_AUCTION_SEARCH_BY_ITEMKEY_GP
{
public:
    // ORIG W 0x0822ea84
    PCK_AUCTION_SEARCH_BY_ITEMKEY_GP();   // 定义 ValueCtors.cpp

    // [b23S3C] 字段化依据同 GA 头（GP 段 local_a7/local_a3 + itemKeys 循环写点）。
    nsl::PACKET_HEADER m_header;     // +0x00
    unsigned int m_uid;              // +0x12
    unsigned int m_characNo;         // +0x16
    TSearchByItemId_ m_search;       // +0x1a..0x2e
    unsigned char m_b2f;             // +0x2f
    unsigned char m_b30;             // +0x30
    unsigned int m_itemKeys[0x14];   // +0x31..0x80
};  // sizeof 0x81
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_SEARCH_BY_ITEMKEY_GP_H_
