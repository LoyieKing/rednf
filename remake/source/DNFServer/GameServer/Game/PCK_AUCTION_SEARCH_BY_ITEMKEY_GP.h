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

    nsl::PACKET_HEADER m_header;   // +0x00
    char m_pad12[0x8];             // +0x12..0x19
    TSearchByItemId_ m_search;     // +0x1a
};  // sizeof 0x81
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_SEARCH_BY_ITEMKEY_GP_H_
