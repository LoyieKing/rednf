#ifndef GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA_H_
#define GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA_H_

// LinkHelper 核销批次：ORIG ctor W @0x0822e736 _ZN32PCK_AUCTION_SEARCH_BY_NOITEMKEY_GAC1Ev
// objdump 转写：PACKET_HEADER ctor -> +0x1a TSearchByCategory_ ctor -> memset(this,0,0x33)
// -> setCategory(0) + setPacketID(7) + setSize(0x33)。
// 布局：nsl::PACKET_HEADER(0x12) + m_data[8] + TSearchByCategory_ @0x1a，sizeof=0x33。
#include "PacketDesign.h"
#include "TSearchByCategory_.h"

struct PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA : public nsl::PACKET_HEADER
{
    char m_data[8];                    // +0x12..+0x19
    TSearchByCategory_ m_search;       // +0x1a（内含 +0xb 处 ROI_Category）
    char m_tail[0x33 - 0x31];          // +0x31

    PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA()
    {
        PACKET_CTOR_BODY(0x0, 0x7, 0x33);
    }
};

#endif  // GAME_PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA_H_
