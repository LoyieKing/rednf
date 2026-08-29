#ifndef GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GA_H_
#define GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GA_H_

// LinkHelper 核销批次：ORIG ctor W @0x0822e4d8 _ZN30PCK_AUCTION_ASK_AVERAGE_PRICE_GAC1Ev
// objdump 转写：PACKET_HEADER ctor -> +0x22 ROI_Category ctor -> memset(this,0,0x2f)
// -> setCategory(0) + setPacketID(1) + setSize(0x2f)。
// 布局：nsl::PACKET_HEADER(0x12) + m_pad1 + ROI_Category @0x22，sizeof=0x2f。
#include "PacketDesign.h"
#include "ROI_Category.h"

struct PCK_AUCTION_ASK_AVERAGE_PRICE_GA : public nsl::PACKET_HEADER
{
    char m_pad1[0x10];        // +0x12..+0x21
    ROI_Category m_roi;       // +0x22（ORIG ctor 显式调用，随后 memset 覆盖等效）
    char m_tail[0x2f - 0x2e]; // +0x2e

    PCK_AUCTION_ASK_AVERAGE_PRICE_GA()
    {
        PACKET_CTOR_BODY(0x0, 0x1, 0x2f);
    }
};

#endif  // GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GA_H_
