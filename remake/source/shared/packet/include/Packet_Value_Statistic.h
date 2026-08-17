//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VALUE_STATISTIC_H
#define PACKET_VALUE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Value_Statistic : public PacketHeader {
public:
    Packet_Value_Statistic() : PacketHeader(0x17ca, 0x86),
        m_level(0)
    {

    };
    int m_level;      // +0xa（InsertValueStatistic 循环下标 1..0x46；statics AddValueStatistics 作 map 键，
                      //      SendDBValueStatistic SQL log_value_stat.level）
    int m_data[0x1e]; // +0xe（0x1e * 4 = 0x78，总 0x86；ValueStatisticData.m_data[0x1e]；
                      //      SendDBValueStatistic SQL log_value_stat 列序：
                      //      0=uv,1=drop_gold,2=drop_item,3=result_card_gold,4=result_card_item,
                      //      5=gold_card_item,6=store_item_buy,7=jar_item,8=disjoint_create,
                      //      9=upgrade_faild_forced_disjoint,10=quest_reward,
                      //      11=deathtower_card_gold,12=deathtower_card_item,
                      //      13=consume_store_item_buy,14=consume_upgrade_attempt,
                      //      15=consume_upgrade_faild,16=consume_stamina_recovery,
                      //      17=consume_quest_consume,18=consume_auction_commision,
                      //      19=consume_item_disjoint,20=consume_item_repair,
                      //      21=consume_item_use,22=consume_item_drop,23=consume_gold_drop,
                      //      24=consume_gold_card_price,25=consume_qp_init）
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Value_Statistic, 0x86);

#endif  // PACKET_VALUE_STATISTIC_H
