//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H
#define PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H

#include "PacketHeader.h"

class Packet_TowerOfDespair_Statistic_GTS : public PacketHeader {
public:
    Packet_TowerOfDespair_Statistic_GTS(): PacketHeader(0x271c,0x17) {};
    unsigned int m_padA;          // +0xa
    int m_layer;                  // +0xe（层数，(0, 0x64]）
    unsigned char m_enterFlag;    // +0x12（0=success++，非 0=enter++ 并记 server）
    unsigned int m_serverId;      // +0x13
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_TowerOfDespair_Statistic_GTS, 0x17);


#endif //PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H
