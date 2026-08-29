#ifndef DNF_GAME_PACKET_RANDOMBOX_STATISTIC_H_
#define DNF_GAME_PACKET_RANDOMBOX_STATISTIC_H_

// HdrCreate3: ORIG exported symbols. ORIG W ctor _ZN26Packet_Randombox_statisticC2Ev
// 0x086945a2（C1/C2 同址弱别名）。ctor 反汇编：PacketHeader(this, 0x17b6, 0xc)；
// movb $0,(this+0xa)；movb $0,(this+0xb)。
#include "PacketHeader.h"

class Packet_Randombox_statistic : public PacketHeader
{
public:
    Packet_Randombox_statistic()
        : PacketHeader(0x17b6, 0xc), m_boxKind(0), m_actionKind(0)
    {
    }

    char m_boxKind;      // +0xa（CUser.cpp SendRandomBoxStatistic）
    char m_actionKind;   // +0xb
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_RANDOMBOX_STATISTIC_H_
