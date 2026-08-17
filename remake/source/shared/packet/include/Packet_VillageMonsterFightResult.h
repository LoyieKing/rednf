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
    Packet_VillageMonsterFightResult() : PacketHeader(0x1772, 0x2b),
        m_result(0)
    {

    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_VillageMonsterFightResult, 0x2b);


#endif //PACKET_VILLAGEMONSTERFIGHTRESULT_H
