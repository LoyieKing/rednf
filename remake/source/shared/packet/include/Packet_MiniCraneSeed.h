//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MINICRANESEED_H
#define PACKET_MINICRANESEED_H

#include "PacketHeader.h"

class Packet_MiniCraneSeed : public PacketHeader {
public:
    unsigned int m_seed;  // +0xa
    Packet_MiniCraneSeed() : PacketHeader(0x27f8, 0xe),
        m_seed(0)
    {

    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_MiniCraneSeed, 0xe);

#endif  // PACKET_MINICRANESEED_H
