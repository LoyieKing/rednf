#ifndef GAME_PCK_AUCTION_MY_AUCTION_HISTORY_GA_H_
#define GAME_PCK_AUCTION_MY_AUCTION_HISTORY_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e87c
// _ZN33PCK_AUCTION_MY_AUCTION_HISTORY_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x1e)
// + setCategory(0x0) + setPacketID(0xa) + setSize(0x1e)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x1e（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_MY_AUCTION_HISTORY_GA : public nsl::PACKET_HEADER
{
    char m_data[0x1e - 0x12];   // +0x12 起（ORIG setSize(0x1e)）

    PCK_AUCTION_MY_AUCTION_HISTORY_GA()
    {
        PACKET_CTOR_BODY(0x0, 0xa, 0x1e);
    }
};

#endif  // GAME_PCK_AUCTION_MY_AUCTION_HISTORY_GA_H_
