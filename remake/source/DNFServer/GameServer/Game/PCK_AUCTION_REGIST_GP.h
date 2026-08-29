#ifndef DNF_GAME_PCK_AUCTION_REGIST_GP_H_
#define DNF_GAME_PCK_AUCTION_REGIST_GP_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CAuction.cpp@27（源码定义复制）
// ORIG W ctor _ZN22PCK_AUCTION_REGIST_GPC2Ev 0x082f7e06（C1/C2 同址弱别名）。

#include "PacketDesign.h"   // nsl::PACKET_HEADER / PACKET_CTOR_BODY 宏

// 拍卖服务器 GP 通道注册包。定义完整复制自 CAuction.cpp:27-30
//（成员/方法/访问级一致；ctor 语义 PACKET_CTOR_BODY(0x12, 0, 0x16)）。
struct PCK_AUCTION_REGIST_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_GP() { PACKET_CTOR_BODY(0x12, 0, 0x16); }
};

#endif  // DNF_GAME_PCK_AUCTION_REGIST_GP_H_
