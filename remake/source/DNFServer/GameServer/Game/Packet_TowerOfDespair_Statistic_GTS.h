// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 复制定义（has_def=1，源 CDungeon.cpp:513-521 / CPartyDepsD2.cpp:1628-1643，
// 两处 cpp 本地同构定义均为 Key 展开前的原始形态）
#ifndef DNF_GAME_PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H_
#define DNF_GAME_PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H_

#include "PacketHeader.h"

// ORIG W 0x08374cba _ZN35Packet_TowerOfDespair_Statistic_GTSC1Ev，总尺寸 0x17：
// PacketHeader(0x271c,0x17)[10] + Key[8] + byte flag + uint accId。
// 注意：CDungeon.cpp:513 与 CPartyDepsD2.cpp:1628 各有本地 struct 定义（均未删除），
// 引入本头的 TU 需避免重复定义；cpp 侧清理由宿主统一安排（冲突已记录）。
// Key 结构（TowerOfDespairStatistic_Key{int m_0; int m_4;}，0x8）在 cpp 为局部类型，
// 此处按等价偏移展开为两个 int 成员以避免与 cpp 局部 Key 重名冲突【推断：偏移一致】。
class Packet_TowerOfDespair_Statistic_GTS
{
public:
    Packet_TowerOfDespair_Statistic_GTS();  // ORIG W 0x08374cba：m_header(0x271c,0x17)，其余清零

    PacketHeader m_header;   // +0x00（packed 0xa 字节）
    int m_key0;              // +0x0a（Key.m_0）
    int m_key4;              // +0x0e（Key.m_4）
    unsigned char m_flag;    // +0x12
    unsigned int m_accId;    // +0x13
} __attribute__((packed));   // sizeof == 0x17

#endif  // DNF_GAME_PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H_
