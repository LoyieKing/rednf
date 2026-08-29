#ifndef GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP_H_
#define GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822eafa
// _ZN34PCK_AUCTION_SEARCH_BY_NOITEMKEY_GPC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x33)
// + setCategory(0x12) + setPacketID(0x7) + setSize(0x33)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x33（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP : public nsl::PACKET_HEADER
{
    char m_data[0x33 - 0x12];   // +0x12 起（ORIG setSize(0x33)）

    PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP()
    {
        PACKET_CTOR_BODY(0x12, 0x7, 0x33);
    }
};

#endif  // GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP_H_
