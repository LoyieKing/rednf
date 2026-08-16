//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEMONSTERFIGHTRESULT_H
#define PACKET_VILLAGEMONSTERFIGHTRESULT_H

#include "PacketHeader.h"

class Packet_VillageMonsterFightResult : public PacketHeader {
public:
    unsigned int m_keys[8];  // +0xa（后 4 个为角色号）
    char m_pad;              // +0x2a
    Packet_VillageMonsterFightResult() : PacketHeader(0x1772, 0x2b),
        m_pad(0)
    {

    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_VillageMonsterFightResult, 0x2b);


#endif //PACKET_VILLAGEMONSTERFIGHTRESULT_H
