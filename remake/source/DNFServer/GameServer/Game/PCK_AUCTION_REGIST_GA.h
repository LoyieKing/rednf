// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CAuction.cpp
// PCK_AUCTION_REGIST_GA：ORIG 弱内联 ctor W @0x082f7d36。
// has_def=1：完整复制定义自 CAuction.cpp:23（继承 nsl::PACKET_HEADER）。
// ctor 体在 CAuction.cpp 内使用 PACKET_CTOR_BODY(0, 0, 0x16) 宏，此处仅声明，
// 定义仍由 CAuction.cpp 提供（本头不改任何 cpp）。
#ifndef DNF_GAME_PCK_AUCTION_REGIST_GA_H_
#define DNF_GAME_PCK_AUCTION_REGIST_GA_H_

#include "PacketDesign.h"   // nsl::PACKET_HEADER 权威定义
struct PCK_AUCTION_REGIST_GA : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_GA();   // ORIG W @0x082f7d36（体：PACKET_CTOR_BODY(0,0,0x16)，CAuction.cpp）
};

#endif  // DNF_GAME_PCK_AUCTION_REGIST_GA_H_
