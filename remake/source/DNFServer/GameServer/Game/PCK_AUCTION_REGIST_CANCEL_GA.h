#ifndef GAME_PCK_AUCTION_REGIST_CANCEL_GA_H_
#define GAME_PCK_AUCTION_REGIST_CANCEL_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e5c4
// _ZN28PCK_AUCTION_REGIST_CANCEL_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x22)
// + setCategory(0x0) + setPacketID(0x4) + setSize(0x22)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x22（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_REGIST_CANCEL_GA : public nsl::PACKET_HEADER
{
    char m_data[0x22 - 0x12];   // +0x12 起（ORIG setSize(0x22)）

    PCK_AUCTION_REGIST_CANCEL_GA()
    {
        PACKET_CTOR_BODY(0x0, 0x4, 0x22);
    }
};

#endif  // GAME_PCK_AUCTION_REGIST_CANCEL_GA_H_
