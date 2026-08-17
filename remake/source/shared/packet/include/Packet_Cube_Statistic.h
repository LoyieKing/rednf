//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CUBE_STATISTIC_H
#define PACKET_CUBE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Cube_Statistic : public PacketHeader {
public:
    unsigned short m_channel;    // +a（sendCubeStatistic: CEnvironment::get_channel_no）
    unsigned short m_level;      // +c（CUserCharacInfo::get_charac_level）
    unsigned int m_itemIndex;    // +e（sendCubeStatistic param_1；statics STCubeStatisticKey.m_index /
                                 //      CubeStatisticItem.m_itemIndex）
    int m_itemCount;             // +12（sendCubeStatistic param_2；statics 累加值 CubeStatisticItem.m_itemCount）
    unsigned char m_type;        // +16（CUBE_STATISTIC_FIELD；statics STCubeStatisticKey.m_type）

    Packet_Cube_Statistic() : PacketHeader(0xc33, 0x17),
        m_channel(0), m_level(0), m_itemIndex(0), m_itemCount(0), m_type(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Cube_Statistic, 0x17);

#endif  // PACKET_CUBE_STATISTIC_H
