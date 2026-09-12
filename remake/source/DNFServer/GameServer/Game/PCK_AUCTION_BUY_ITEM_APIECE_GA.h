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
#include <cstddef>      // offsetof（下方布局静态断言）

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
        // [R22a02 2026-09-13 ④ 修复·类型轴] ORIG 读点（0x08214fe4 / 0x08215023 /
        //   0x08215090 / 0x08215142）为**单条 disp 直读** `mov -0x3c(%ebp),%edx/%eax`
        //   ⇒ 原码该成员是 **int 值读**；旧 char[4] 视图解引用令 4.4 在 4 处读各多
        //   `lea;add`（本件全部 gap −8）。但 packed 成员的取址指针带 packed 属性，
        //   `*reinterpret_cast<int*>(&member)` 仍被判「绑定紧实字段到 int&」⇒ 采用
        //   匿名 union 双视角（P246/P235 范式）：int 视角供值读（单条 disp），
        //   char[4] 视角供 `*reinterpret_cast<int*>(...)` 传 get_int(int&)（取址三连，
        //   ORIG 0x08214e9f lea+add+get_int 同形）。sizeof/偏移逐字节不变（下方断言）。
        union {
            int m_price;        // payload +0x08（绝对 +0x1a）·价格 int（AuctionPacket.h:222 `int price; // @26`）
            char m_priceBytes[4];   // 同址字节视角（get_int(int&) 取址用）
    };
        char m_key[8];          // payload +0x0c..0x13（绝对 +0x1e..+0x25）
        union {
            int m_count;        // 绝对 +0x26 非对齐 int（ORIG add $0x26）
            char m_countBytes[4];   // 同址字节视角（get_int(int&) 取址用）
    };
        char m_tail[4];         // 补齐 payload 总长 0x18
    };
    // [R22a02 2026-09-13] 布局不变证明（编译期强制，值 = 改动前实测布局）。
    static_assert(sizeof(Body) == 0x1c, "Body size changed");
    static_assert(offsetof(Body, m_price) == 0x08, "m_price offset changed");
    static_assert(offsetof(Body, m_key) == 0x0c, "m_key offset changed");
    static_assert(offsetof(Body, m_count) == 0x14, "m_count offset changed");
    static_assert(offsetof(Body, m_tail) == 0x18, "m_tail offset changed");
    union {
        char m_data[0x2a - 0x12];   // +0x12 起（ORIG setSize(0x2a)）
        Body m_body;
    };

    PCK_AUCTION_BUY_ITEM_APIECE_GA()
    {
        PACKET_CTOR_BODY(0x0, 0xe, 0x2a);
    }
};

// payload 18B（Body 0x1c 另含 m_tail 尾 4B）—— 与改动前同式，证明 union 双视角零尺寸影响。
static_assert(sizeof(nsl::PACKET_HEADER) + 0x1c == sizeof(PCK_AUCTION_BUY_ITEM_APIECE_GA),
              "packet size changed");

#endif  // GAME_PCK_AUCTION_BUY_ITEM_APIECE_GA_H_
