//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DEATHTOWER_STATISTIC_VALUE_H
#define PACKET_DEATHTOWER_STATISTIC_VALUE_H

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Value : public PacketHeader {
public:
    unsigned char m_type;       // +0xa（发送 SendDeathTowerValueStatistic param_3；接收 key.m_type；DB log_deathtower_value.type）
    unsigned short m_level;     // +0xb（param_4；key.m_level；DB level）
    unsigned int m_counterIdx;  // +0xd（param_2：字段 10 时映射 3..6，否则原值；key.m_counterIdx；DB 计数器下标）
    int m_value;                // +0x11（param_5；ValueStatistic.m_data[0]；DBMW STDeathTowerValueWireItem.m_value）

    Packet_DeathTower_Statistic_Value(): PacketHeader(0xc3b,0x15) {};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_DeathTower_Statistic_Value, 0x15);


#endif //PACKET_DEATHTOWER_STATISTIC_VALUE_H
