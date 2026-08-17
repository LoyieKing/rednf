//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CIRCULATION_STATISTIC_H
#define PACKET_CIRCULATION_STATISTIC_H

#include "PacketHeader.h"

class Packet_Circulation_Statistic : public PacketHeader {
public:
    int m_level;      // +0xa（InsertCirculationStatistic 循环 level 1..0x46；DB log_gold_stat.level）
    int m_data[0x30]; // +0xe（0x30 * 4 = 0xc0，总 0xce；ORIG 仅访达 0xce）
    unsigned short m_channel; // +0xce（InsertCirculationStatistic: CEnvironment::get_channel_no）

    Packet_Circulation_Statistic() : PacketHeader(0x17cb, 0xd0),
        m_level(0), m_channel(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Circulation_Statistic, 0xd0);

#endif  // PACKET_CIRCULATION_STATISTIC_H
