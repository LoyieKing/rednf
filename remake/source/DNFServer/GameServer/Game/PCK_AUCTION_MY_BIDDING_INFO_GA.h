#ifndef GAME_PCK_AUCTION_MY_BIDDING_INFO_GA_H_
#define GAME_PCK_AUCTION_MY_BIDDING_INFO_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e814
// _ZN30PCK_AUCTION_MY_BIDDING_INFO_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x1a)
// + setCategory(0x0) + setPacketID(0x9) + setSize(0x1a)。
// 布局：nsl::PACKET_HEADER(0x12) + payload，sizeof=0x1a（ORIG memset/setSize 长度）。
// 载荷成员化（R12a01，P367/P171）：ORIG Dispatcher_AuctionMyBiddingInfo::dispatch_sig
// 0x08216c48 `mov %eax,-0x24(%ebp)` / 0x08216c56 `mov %eax,-0x20(%ebp)`，按槽基 -0x36
// 换算 = packet+0x12（GetUID）/ packet+0x16（getCurCharacNo）⇒ 原码是成员直存（EBP disp），
// 我方旧 `char m_data[8]` + 裸偏移写被 4.4 折成 `lea -0x2e(%ebp); lea 0x12(%eax); mov (%edx)`
// 两步。字段名按写入来源取；Auction 服务器侧权威头 AuctionPacket.h:196 同偏移名为
// char_idx@18(+0x12) / buyer_id@22(+0x16)（本协议两侧同名不同义，以 ORIG 写入值为准）。
#include "PacketDesign.h"

#pragma pack(push, 1)
struct PCK_AUCTION_MY_BIDDING_INFO_GA : public nsl::PACKET_HEADER
{
    unsigned int m_uid;         // +0x12（ORIG GetUID 写入）
    unsigned int m_characNo;    // +0x16（ORIG getCurCharacNo 写入）

    PCK_AUCTION_MY_BIDDING_INFO_GA()
    {
        PACKET_CTOR_BODY(0x0, 0x9, 0x1a);
    }
};
#pragma pack(pop)

#endif  // GAME_PCK_AUCTION_MY_BIDDING_INFO_GA_H_
