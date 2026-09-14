//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SECRET_SHOP_STATISTIC_H
#define PACKET_SECRET_SHOP_STATISTIC_H

#include "PacketHeader.h"
// 元素类型 SECRET_SHOP_STATISTIC_DATA（0x14）的唯一声明点 = Statics/GMAccounts.h，
// 此处只 include，不再抄第二份（同 Game/Packet_Secret_Shop_Statistic.h 的
// `#include "../Statics/GMAccounts.h"` 口径；shared 包头 include 服务头的相对路径
// 写法同 Packet_Guild_Guild_Cargo_Push_Item.h）。
#include "../../../DNFServer/GameServer/Statics/GMAccounts.h"

class Packet_Secret_Shop_Statistic : public PacketHeader {
public:
    int m_count;     // +0xa（SendSecretShopStatistic: map size；ORIG ctor 只显式清此字段）
    int m_shopIdx;   // +0xe（SendSecretShopStatistic 循环索引 0..2）
    // +0x12..+0xfb1：SECRET_SHOP_STATISTIC_DATA[200]，0x14/条 = 5 int：
    //      +0=dungeon_idx(map 键)、+4=show_count、+8=show_charac_count、
    //      +c=buy_count、+10=price，对应 DB log_secret_shop 列；
    //      df_statics_r AddSecretShopStatistic 按 m_data[0] 聚合后累加 1..4
    //      （ORIG 0x80745d8 实证：元素 i 基址 pkt+0x12+0x14*i，m_data[3] 读 +0x1e）。
    // 必须是 SECRET_SHOP_STATISTIC_DATA 数组（不是 char[]）：ORIG ctor 0x80758ae 在
    // m_count=0 后对 pkt+0x12 起 200 个元素逐个调用元素 ctor（0x8075878，五 int 清零），
    // 步长 0x14 —— 只有非平凡元素类型的隐式成员构造才会生成这段循环。
    SECRET_SHOP_STATISTIC_DATA m_items[200];

    Packet_Secret_Shop_Statistic() : PacketHeader(0x1b69, 0xfb2),
        m_count(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Secret_Shop_Statistic, 0xfb2);

#endif  // PACKET_SECRET_SHOP_STATISTIC_H
