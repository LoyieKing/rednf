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
        char m_pad0x12[0x14];   // payload +0x00..+0x13（绝对 +0x12..+0x25）
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
