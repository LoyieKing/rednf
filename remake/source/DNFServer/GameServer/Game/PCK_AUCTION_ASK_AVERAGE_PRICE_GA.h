#ifndef GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GA_H_
#define GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GA_H_

// LinkHelper 核销批次：ORIG ctor W @0x0822e4d8 _ZN30PCK_AUCTION_ASK_AVERAGE_PRICE_GAC1Ev
// objdump 转写：PACKET_HEADER ctor -> +0x22 ROI_Category ctor -> memset(this,0,0x2f)
// -> setCategory(0) + setPacketID(1) + setSize(0x2f)。
// 布局：nsl::PACKET_HEADER(0x12) + m_pad1 + ROI_Category @0x22，sizeof=0x2f。
#include "PacketDesign.h"
#include "ROI_Category.h"

#pragma pack(push, 1)
// ORIG ctor `add $0x22,%eax`（ROI_Category ctor 实参）实证 m_roi@+0x22；
// ROI_Category 类对齐 4 会把 m_roi 推到 0x24，外层须 pack(1)（sizeof 0x2f 不变）。
struct PCK_AUCTION_ASK_AVERAGE_PRICE_GA : public nsl::PACKET_HEADER
{
    unsigned int m_uid;       // +0x12
    int m_characNo;           // +0x16
    unsigned int m_itemIdx;   // +0x1a
    char m_flag;              // +0x1e
    char m_upgrade[3];        // +0x1f
    ROI_Category m_roi;       // +0x22
    char m_tail;              // +0x2e

    PCK_AUCTION_ASK_AVERAGE_PRICE_GA()
    {
        PACKET_CTOR_BODY(0x0, 0x1, 0x2f);
    }
};
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_ASK_AVERAGE_PRICE_GA_H_
