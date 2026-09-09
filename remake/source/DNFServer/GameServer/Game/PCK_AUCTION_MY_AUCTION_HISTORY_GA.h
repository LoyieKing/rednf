#ifndef GAME_PCK_AUCTION_MY_AUCTION_HISTORY_GA_H_
#define GAME_PCK_AUCTION_MY_AUCTION_HISTORY_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e87c
// _ZN33PCK_AUCTION_MY_AUCTION_HISTORY_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x1e)
// + setCategory(0x0) + setPacketID(0xa) + setSize(0x1e)。
// 布局：nsl::PACKET_HEADER(0x12) + payload m_data，sizeof=0x1e（ORIG memset/setSize 长度）。
#include "PacketDesign.h"

// [b28d-solo-Dispatchers] P142/P250：ORIG 0x082087xx 栈槽基址 -0x26（非 4 对齐，
// sizeof 0x1e）⇒ 类有效对齐为 1（4.4 默认 4 对齐给 -0x28，差 2）。pack(1) 使
// 4.4 复现 -0x26；成员偏移/sizeof 不变（0x12/0x16/0x1a/0x1e 已按 ORIG 核对）。
#pragma pack(push, 1)
struct PCK_AUCTION_MY_AUCTION_HISTORY_GA : public nsl::PACKET_HEADER
{
    // [b28d-solo-Dispatchers] §8.3 字段化（证据：Dispatcher_AuctionMyAuctionHistory::
    //   dispatch_sig ORIG 0x082087xx 三次 `call; mov %eax,-0x14/-0x10/-0xc(%ebp)`，
    //   ga 基址 -0x26 ⇒ 载荷字段位于 +0x12/+0x16/+0x1a；sizeof 仍 0x1e）。
    unsigned int m_uid;         // +0x12（user->GetUID()）
    unsigned int m_characNo;    // +0x16（getCurCharacNo()）
    unsigned int m_accId;       // +0x1a（get_acc_id()）

    PCK_AUCTION_MY_AUCTION_HISTORY_GA()
    {
        PACKET_CTOR_BODY(0x0, 0xa, 0x1e);
    }
};
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_MY_AUCTION_HISTORY_GA_H_
