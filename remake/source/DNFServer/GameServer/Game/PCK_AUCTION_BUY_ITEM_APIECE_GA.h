#ifndef GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_
#define GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e8e4
// _ZN30PCK_AUCTION_BUY_ITEM_APIECE_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x2a)
// + setCategory(0x0) + setPacketID(0xe) + setSize(0x2a)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x2a（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_BUY_ITEM_APIECE_GA : public nsl::PACKET_HEADER
{
    char m_data[0x2a - 0x12];   // +0x12 起（ORIG setSize(0x2a)）

    PCK_AUCTION_BUY_ITEM_APIECE_GA()
    {
        PACKET_CTOR_BODY(0x0, 0xe, 0x2a);
    }
};

#endif  // GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_
