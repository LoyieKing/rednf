#ifndef GAME_PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP_H_
#define GAME_PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP_H_

// ORIG W 0x0822eb70：PACKET_HEADER ctor + memset(this,0,0x1a)
// + setCategory(0x12) + setPacketID(8) + setSize(0x1a)。
#include "PacketDesign.h"

struct PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP : public nsl::PACKET_HEADER
{
    char m_data[0x1a - 0x12];

    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP()
    {
        PACKET_CTOR_BODY(0x12, 0x8, 0x1a);
    }
};

#endif
