#ifndef GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP_H_
#define GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822eafa
// _ZN34PCK_AUCTION_SEARCH_BY_NOITEMKEY_GPC1Ev，objdump 逐指令转写：
// PACKET_HEADER ctor + TSearchByCategory_C1(this+0x1a) + memset(this,0,0x33)
// + setCategory(0x12) + setPacketID(0x7) + setSize(0x33)。
// 布局：nsl::PACKET_HEADER(0x12) + m_pad12(0x8) + m_cat(0x17@0x1a) + m_pad31(0x2)，sizeof=0x33。
#include "PacketDesign.h"
#include "TSearchByCategory_.h"

struct PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP : public nsl::PACKET_HEADER
{
    char m_pad12[0x8];           // +0x12..+0x19
    TSearchByCategory_ m_cat;    // +0x1a..+0x30（ORIG TSearchByCategory_C1 在 this+0x1a）
    char m_pad31[0x33 - 0x31];   // +0x31..+0x32

    PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP() : m_cat()
    {
        PACKET_CTOR_BODY(0x12, 0x7, 0x33);
    }
};

#endif  // GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP_H_
