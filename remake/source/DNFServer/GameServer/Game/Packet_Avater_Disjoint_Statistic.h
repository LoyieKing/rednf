// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 定义提取自 Game/CompoundDeathLCB.cpp:80-102（该 TU 唯一定义，
// 接入时由宿主迁除 cpp 内重复段）。字段偏移按 ORIG _SendDisjointAvatarStatistic
// 写入序建模，sizeof == 0x28。PacketHeader ctor W 0x08110b70。
#ifndef DNF_GAME_PACKET_AVATER_DISJOINT_STATISTIC_H_
#define DNF_GAME_PACKET_AVATER_DISJOINT_STATISTIC_H_

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Avater_Disjoint_Statistic : public PacketHeader
{
public:
    // ORIG W 0x08110bae：PacketHeader(0x17a2, 0x28) 后仅清零 dword @+0xa。
    Packet_Avater_Disjoint_Statistic()
        : PacketHeader(static_cast<unsigned short>(0x17a2),
                       static_cast<unsigned short>(0x28))
    {
        m_field0a = 0;
    }

    unsigned int m_field0a;       // +0x0a（调用方写 2）
    int m_gradeIdxA;              // +0x0e rarity==2?2:grade-1
    int m_subTypeA;               // +0x12 scriptA vtbl+0xc 结果
    unsigned char m_flagA;        // +0x16 置 0
    unsigned int m_gradeIdxB;              // +0x1b 同 A 组算法但取 scriptB grade
    int m_subTypeB;               // +0x1f【ORIG 怪癖】也读 scriptA vtbl+0xc
    unsigned char m_flagB;        // +0x23 置 0
    unsigned int m_pad24;         // +0x24..+0x27（ORIG SendPacket 传 0x28，尾部未初始化）
};                                // sizeof == 0x28（pack(1) 下需显式尾填充）
#pragma pack(pop)

#endif  // DNF_GAME_PACKET_AVATER_DISJOINT_STATISTIC_H_
