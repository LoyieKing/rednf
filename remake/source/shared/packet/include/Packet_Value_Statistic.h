//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VALUE_STATISTIC_H
#define PACKET_VALUE_STATISTIC_H

#include "PacketHeader.h"

class Packet_Value_Statistic : public PacketHeader {
public:
    Packet_Value_Statistic() : PacketHeader(0x17ca, 0x86),
        m_f0a(0)
    {

    };
    int m_f0a;        // +0xa（find/make_pair 键）
    int m_data[0x1e]; // +0xe（0x1e * 4 = 0x78，总 0x86）
} __attribute__((packed));

#endif  // PACKET_VALUE_STATISTIC_H
