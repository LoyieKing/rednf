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

    // 字段化（CUserMain 2026-09-04，与 shared/packet/include 权威版对齐命名；
    // 证据 CUser::DisConnSig @0x0864927x 区：+0xa movb $1、+0xb movb hwSpec[1]、
    // +0xc `mov %eax` 4B（param_4）、+0x10 `mov %eax,-0xcb2(%ebp)` 4B 槽写
    // GetLastLoginChannelNo —— 高 2 字节随后被 +0x12 memcpy 覆盖，2B/4B 语义等价，
    // 取 4B 忠实 ORIG store）。
    union { unsigned char m_type;  char m_padA; };   // +0x0a（ctor 未写）
    union { unsigned char m_count; char m_flagB; };  // +0x0b（ctor 置 0）
    union
    {
        char m_padC[6];        // +0x0c..+0x11
        struct
        {
            int m_errorCode;   // +0x0c
            // 权威版（shared/packet/include）声明 ushort；ORIG DisConnSig 0x086492aa
            // 为 4B 槽写（mov %eax,-0xcb2），高 2 字节恒 0 且随后被 +0x12 memcpy
            // 覆盖（count=0 时 ctor 已清零该区）⇒ 2B 声明语义全覆盖。
            short m_errorLine; // +0x10
        };
    };
    HWSpec m_specs[0xff];    // +0x12..+0xc05（stride 0xc）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Client_Spec_Statistic) == 0xc06, "Packet_Client_Spec_Statistic layout");

#endif  // DNF_GAME_PACKET_CLIENT_SPEC_STATISTIC_H_
