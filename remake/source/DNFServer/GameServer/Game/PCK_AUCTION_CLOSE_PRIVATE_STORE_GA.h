#ifndef GAME_PCK_AUCTION_CLOSE_PRIVATE_STORE_GA_H_
#define GAME_PCK_AUCTION_CLOSE_PRIVATE_STORE_GA_H_

// ORIG ctor 0x85cb3dc（W，C2 同址）：PACKET_HEADER C1 → memset(this,0,0x16) →
// setCategory(0)/setPacketID(0xc)/setSize(0x16)。sizeof 0x16。
#include "PacketDesign.h"

#pragma pack(push, 1)
class PCK_AUCTION_CLOSE_PRIVATE_STORE_GA
{
public:
    // ORIG 0x85cb3dc _ZN34PCK_AUCTION_CLOSE_PRIVATE_STORE_GAC1Ev (size 0x00000067)
    PCK_AUCTION_CLOSE_PRIVATE_STORE_GA();   // 定义 OnlyOrigBatch4_Force.cpp

    nsl::PACKET_HEADER m_header;   // +0x00（0x12）
    // +0x12：ORIG 调用点写 memberIdentificationNumber_t（4 字节，AuctionPacket.h
    // 权威镜像 `m_id` @+0x12）；union 保留字节视图不引入新符号、sizeof 不变。
    union
    {
        char m_body[0x16 - 0x12];
        unsigned int m_id;
    };
};  // sizeof 0x16
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_CLOSE_PRIVATE_STORE_GA_H_
