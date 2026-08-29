#ifndef GAME_PCK_AUCTION_ASK_OWNER_IS_VIP_GP_H_
#define GAME_PCK_AUCTION_ASK_OWNER_IS_VIP_GP_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822ebd8
// _ZN31PCK_AUCTION_ASK_OWNER_IS_VIP_GPC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x37)
// + setCategory(0x12) + setPacketID(0xe) + setSize(0x37)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x37（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

struct PCK_AUCTION_ASK_OWNER_IS_VIP_GP : public nsl::PACKET_HEADER
{
    char m_data[0x37 - 0x12];   // +0x12 起（ORIG setSize(0x37)）

    PCK_AUCTION_ASK_OWNER_IS_VIP_GP()
    {
        PACKET_CTOR_BODY(0x12, 0xe, 0x37);
    }
};

#endif  // GAME_PCK_AUCTION_ASK_OWNER_IS_VIP_GP_H_
