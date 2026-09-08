//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEMONSTERFIGHTRESULT_H
#define PACKET_VILLAGEMONSTERFIGHTRESULT_H

#include "PacketHeader.h"

class Packet_VillageMonsterFightResult : public PacketHeader {
public:
    // b27 Lane20 P223 定稿：双独立数组成员视图（4.4 对同数组 i/i+4 索引恒折
    // lea disp8，仅双成员 u[i]/c[i] 出 ORIG 的 `add $0x4,%edx` 形）。
    union {
        unsigned int m_keys[8];              // +0xa（后 4 个为角色号）
        struct { unsigned int m_pad[4]; unsigned int m_characNos[4]; };
    };
    unsigned char m_result;  // +0x2a（SendVillageMonsterFightResult 的 bool 参数，战斗结果；同步写入 party 0xf9 包）
    // ORIG W 0x086b4b84：PacketHeader(0x1772, 0x2b) + m_result 清零 +
    // m_keys[8] 清零循环 i=0..3 每次清 m_keys[i]/m_keys[i+4]。
    Packet_VillageMonsterFightResult() : PacketHeader(0x1772, 0x2b),
        m_result(0)
    {
        // b27 Lane20 P223：ORIG 0x86b4b84 循环体为 `movl $0,0xa(,%edx,4)` +
        // `add $0x4,%edx` 索引对形 —— 4.4 仅对「u[i] / c[i] 独立数组索引」降级出此形。
        for (int i = 0; i <= 3; ++i)
        {
            m_keys[i] = 0;
            m_characNos[i] = 0;
        }
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_VillageMonsterFightResult, 0x2b);


#endif //PACKET_VILLAGEMONSTERFIGHTRESULT_H
