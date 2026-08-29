#ifndef GAME_LIMITNPCBUYITEMREQUESTINFO_H_
#define GAME_LIMITNPCBUYITEMREQUESTINFO_H_

// ORIG W ctor 0x084b41fe（C2 同址）：仅 PacketHeader(0x27d8, 0x000a)（10 字节头，
// 无其它字段；packed sizeof 0xa）。定义 OnlyOrigBatch4_Force.cpp。
#include "PacketHeader.h"

class LimitNpcBuyItemRequestInfo : public PacketHeader
{
public:
    // ORIG 0x84b41fe _ZN26LimitNpcBuyItemRequestInfoC1Ev (size 0x00000023)
    LimitNpcBuyItemRequestInfo();
} __attribute__((packed));

#endif  // GAME_LIMITNPCBUYITEMREQUESTINFO_H_
