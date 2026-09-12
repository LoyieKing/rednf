#ifndef GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_
#define GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822e8e4
// _ZN30PCK_AUCTION_BUY_ITEM_APIECE_GAC1Ev，objdump 逐指令转写：PACKET_HEADER ctor + memset(this,0,0x2a)
// + setCategory(0x0) + setPacketID(0xe) + setSize(0x2a)。
// 布局：nsl::PACKET_HEADER(0x12) + payload，sizeof=0x2a（ORIG memset/setSize 长度）。
// 字段化证据：Dispatcher_AuctionBuyItemApiece/dispatch_sig.md 08214ee7 `add $0x26,%eax`
// → get_int(int&)，即 payload+0x14（绝对 +0x26）处的非对齐 int 计数（权威名 count，
// 对照 AuctionServer 侧 auction_header/AuctionPacket.h:221 PCK_AUCTION_BUY_ITEM_APIECE_GA）。
// 非对齐 int 不可绑 get_int(int&)，故用 char[4] 字节视图（§4.3 正解），消费侧
// *reinterpret_cast<int*>(…) 取。m_data 保留以兼容同结构其余偏移访问。
#include "PacketDesign.h"

struct PCK_AUCTION_BUY_ITEM_APIECE_GA : public nsl::PACKET_HEADER
{
    struct __attribute__((packed)) Body {
        // [G5DsigE 2026-09-11] Body 视图具名化（偏移/总长逐点不变）：ORIG pck 基址
        //   -0x56 处 +0x12/+0x16/+0x1e/+0x26 均 disp 直访 ⇒ uid/characNo/key 具名。
        unsigned int m_uid;     // payload +0x00（绝对 +0x12）
        int m_characNo;         // payload +0x04（绝对 +0x16）
        // [V331b06 2026-09-12 ④ 修复] 原误标为 m_pad08[4]（pad）。ORIG 实证此处是
        //   get_int 的写入目标、且随整包转发：Dispatcher_AuctionBuyItemApiece::dispatch_sig
        //   0x8214e9f `lea -0x56(%ebp),%eax` + 0x8214ea2 `add $0x1a,%eax` +
        //   0x8214eaf `call PacketBuf::get_int(int&)`；发包点 0x8215081 `lea -0x56(%ebp),%edx`
        //   → CAuctionServerProxy::SendPacket(&pck)。接收侧读该字段：
        //   ServerLab/Auction/AuctionServer/auction_source/HandlerFor_GA_.cpp:642 `pPck->price`；
        //   权威布局 AuctionServer/auction_header/AuctionPacket.h:222 `int price; // @26`。
        // 非对齐 int（+0x1a 非 4 对齐，packed 成员不可直接绑定 int&）⇒ 与 m_count 同法用
        // 4 字节具名视图，消费侧 `*reinterpret_cast<int*>(m_price)`。
        char m_price[4];        // payload +0x08（绝对 +0x1a）·价格 int（AuctionPacket.h:222 `int price; // @26`）
        char m_key[8];          // payload +0x0c..+0x13（绝对 +0x1e..+0x25）
        char m_count[4];        // 绝对 +0x26 非对齐 int（ORIG add $0x26）
        char m_tail[4];         // 补齐 payload 总长 0x18
    };
    union {
        char m_data[0x2a - 0x12];   // +0x12 起（ORIG setSize(0x2a)）
        Body m_body;
    };

    PCK_AUCTION_BUY_ITEM_APIECE_GA()
    {
        PACKET_CTOR_BODY(0x0, 0xe, 0x2a);
    }
};

#endif  // GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_
