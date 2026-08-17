//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SECRET_SHOP_STATISTIC_H
#define PACKET_SECRET_SHOP_STATISTIC_H

#include "PacketHeader.h"

class Packet_Secret_Shop_Statistic : public PacketHeader {
public:
    int m_count;     // +0xa（GMAccounts.h 同包定义 m_count；SendSecretShopStatistic: map size）
    int m_shopIdx;   // +0xe（GMAccounts.h m_shopIdx；SendSecretShopStatistic 循环索引 0..2）
    char m_items[0xfa0];  // +0x12..+0xfb1（SECRET_SHOP_STATISTIC_DATA[200]，0x14/条 = 5 int：
                          //      +0=dungeon_idx(map 键)、+4=show_count、+8=show_charac_count、
                          //      +c=buy_count、+10=price，对应 DB log_secret_shop 列；
                          //      df_statics_r AddSecretShopStatistic 按 m_data[0] 聚合后累加 1..4）

    Packet_Secret_Shop_Statistic() : PacketHeader(0x1b69, 0xfb2),
        m_count(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Secret_Shop_Statistic, 0xfb2);

#endif  // PACKET_SECRET_SHOP_STATISTIC_H
