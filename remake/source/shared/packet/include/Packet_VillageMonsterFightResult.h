//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEMONSTERFIGHTRESULT_H
#define PACKET_VILLAGEMONSTERFIGHTRESULT_H

#include "PacketHeader.h"

class Packet_VillageMonsterFightResult : public PacketHeader {
public:
    unsigned int m_keys[8];  // +0xa（后 4 个为角色号）
    unsigned char m_result;  // +0x2a（SendVillageMonsterFightResult 的 bool 参数，战斗结果；同步写入 party 0xf9 包）
    // ORIG W 0x086b4b84：PacketHeader(0x1772, 0x2b) + m_result 清零 +
    // m_keys[8] 清零循环 i=0..3 每次清 m_keys[i]/m_keys[i+4]。
    Packet_VillageMonsterFightResult() : PacketHeader(0x1772, 0x2b),
        m_result(0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            m_keys[i] = 0;
            m_keys[i + 4] = 0;
        }
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_VillageMonsterFightResult, 0x2b);


#endif //PACKET_VILLAGEMONSTERFIGHTRESULT_H
