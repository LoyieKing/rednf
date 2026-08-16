//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CIRCULATION_STATISTIC_H
#define PACKET_CIRCULATION_STATISTIC_H

#include "PacketHeader.h"

class Packet_Circulation_Statistic : public PacketHeader {
public:
    int m_f0a;        // +0xa
    int m_data[0x30]; // +0xe（0x30 * 4 = 0xc0，总 0xce；ORIG 仅访达 0xce）
    unsigned short m_channel; // +0xce（ctor 清零）

    Packet_Circulation_Statistic() : PacketHeader(0x17cb, 0xd0),
        m_f0a(0), m_fieldce(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Circulation_Statistic, 0xd0);

#endif  // PACKET_CIRCULATION_STATISTIC_H
