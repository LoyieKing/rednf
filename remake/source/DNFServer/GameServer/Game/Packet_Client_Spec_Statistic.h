#ifndef DNF_GAME_PACKET_CLIENT_SPEC_STATISTIC_H_
#define DNF_GAME_PACKET_CLIENT_SPEC_STATISTIC_H_

// LinkHelper 核销批次：ORIG ctor W @0x0822bc18 _ZN28Packet_Client_Spec_StatisticC1Ev
// objdump 转写：PacketHeader(0xc06,0x9ce) + movb $0x0,0xb + 0xfe× HWSpec ctor @+0x12
// （stride 0xc，ORIG 0x0822bc4f 循环 call _ZN6HWSpecC1Ev，末次止于 +0xbfa）。
// 布局：+0xa pad / +0xb flag / +0xc pad / +0x12 HWSpec[0xfe] / +0xc02 tail。sizeof=0xc06。
#include "PacketHeader.h"
#include "HWSpec.h"

#pragma pack(push, 1)

class Packet_Client_Spec_Statistic : public PacketHeader
{
public:
    Packet_Client_Spec_Statistic();   // ORIG W @0x0822bc18

    char   m_padA;           // +0x0a（ctor 未写）
    char   m_flagB;          // +0x0b（ctor 置 0）
    char   m_padC[6];        // +0x0c..+0x11
    HWSpec m_specs[0xfe];    // +0x12..+0xc01（stride 0xc）
    char   m_tail[0xc];       // +0xbfa..+0xc05（ORIG ctor 未写）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Client_Spec_Statistic) == 0xc06, "Packet_Client_Spec_Statistic layout");

#endif  // DNF_GAME_PACKET_CLIENT_SPEC_STATISTIC_H_
