#ifndef GAME_PCK_AUCTION_MY_BIDDING_INFO_GA_H_
#define GAME_PCK_AUCTION_MY_BIDDING_INFO_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e814
// _ZN30PCK_AUCTION_MY_BIDDING_INFO_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x1a)
// + setCategory(0x0) + setPacketID(0x9) + setSize(0x1a)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x1a（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_MY_BIDDING_INFO_GA : public nsl::PACKET_HEADER
{
    char m_data[0x1a - 0x12];   // +0x12 起（ORIG setSize(0x1a)）

    PCK_AUCTION_MY_BIDDING_INFO_GA()
    {
        PACKET_CTOR_BODY(0x0, 0x9, 0x1a);
    }
};

#endif  // GAME_PCK_AUCTION_MY_BIDDING_INFO_GA_H_
