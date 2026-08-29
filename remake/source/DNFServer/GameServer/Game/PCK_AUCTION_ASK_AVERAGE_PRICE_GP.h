#ifndef GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GP_H_
#define GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GP_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e94c
// _ZN32PCK_AUCTION_ASK_AVERAGE_PRICE_GPC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x1f)
// + setCategory(0x12) + setPacketID(0x1) + setSize(0x1f)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x1f（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_ASK_AVERAGE_PRICE_GP : public nsl::PACKET_HEADER
{
    char m_data[0x1f - 0x12];   // +0x12 起（ORIG setSize(0x1f)）

    PCK_AUCTION_ASK_AVERAGE_PRICE_GP()
    {
        PACKET_CTOR_BODY(0x12, 0x1, 0x1f);
    }
};

#endif  // GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GP_H_
