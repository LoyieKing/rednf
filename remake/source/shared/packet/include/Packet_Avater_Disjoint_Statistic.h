//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_AVATER_DISJOINT_STATISTIC_H
#define PACKET_AVATER_DISJOINT_STATISTIC_H

#include "PacketHeader.h"

class Packet_Avater_Disjoint_Statistic : public PacketHeader {
public:
    Packet_Avater_Disjoint_Statistic(): PacketHeader(0x17a2,0x28) {};
    int m_count;                  // +0xa（0..2）
    struct __attribute__((packed)) Item {
        int m_a;                  // +0
        int m_b;                  // +4
        char m_c;                 // +8
        int m_d;                  // +9
    } m_items[2];                 // +0xe，每项 0xd，两项至 +0x28
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Avater_Disjoint_Statistic, 0x28);


#endif //PACKET_AVATER_DISJOINT_STATISTIC_H
