#ifndef GAME_PCK_AUCTION_OPEN_PRIVATE_STORE_GA_H_
#define GAME_PCK_AUCTION_OPEN_PRIVATE_STORE_GA_H_

// ORIG ctor 0x85cb374（W，C2 同址）：PACKET_HEADER C1 → memset(this,0,0xaa) →
// setCategory(0)/setPacketID(0xb)/setSize(0xaa)。sizeof 0xaa。
#include "PacketDesign.h"

#pragma pack(push, 1)
class PCK_AUCTION_OPEN_PRIVATE_STORE_GA
{
public:
    // ORIG 0x85cb374 _ZN33PCK_AUCTION_OPEN_PRIVATE_STORE_GAC1Ev (size 0x00000067)
    PCK_AUCTION_OPEN_PRIVATE_STORE_GA();   // 定义 OnlyOrigBatch4_Force.cpp

    nsl::PACKET_HEADER m_header;   // +0x00（0x12）
    char m_body[0xaa - 0x12];      // +0x12
};  // sizeof 0xaa
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_OPEN_PRIVATE_STORE_GA_H_
