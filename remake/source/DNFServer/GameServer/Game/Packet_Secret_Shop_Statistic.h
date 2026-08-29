#ifndef DNF_GAME_PACKET_SECRET_SHOP_STATISTIC_H_
#define DNF_GAME_PACKET_SECRET_SHOP_STATISTIC_H_

// Packet_Secret_Shop_Statistic：ORIG 弱内联 ctor W @0x085fd490。
// ctor 反汇编：PacketHeader(this, 0x1b69, 0xfb2)；movl $0,(this+0xa)；
// 之后循环 200 次（esi=0xc7 递减）对 this+0x12+i*0x14 逐元素调用
// SECRET_SHOP_STATISTIC_DATA::SECRET_SHOP_STATISTIC_DATA()（0x85fd45a，
// 五个 int 逐个清零）。元素构造由 SECRET_SHOP_STATISTIC_DATA 默认 ctor 承担，
// Packet ctor 本身只需清 m_count（ORIG +0xa 4 字节写即 m_count）。
#include "PacketHeader.h"
#include "../Statics/GMAccounts.h"

class Packet_Secret_Shop_Statistic : public PacketHeader
{
public:
    Packet_Secret_Shop_Statistic()
        : PacketHeader(0x1b69, 0xfb2), m_count(0)
    {
    }

    int m_count;  // +0xa（ORIG movl $0 清零）
    SECRET_SHOP_STATISTIC_DATA m_items[200];  // +0x12，每元素 0x14
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_SECRET_SHOP_STATISTIC_H_
