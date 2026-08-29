// LinkHelper 核销批次 —— Packet_Client_Spec_Statistic ctor（ORIG W @0x0822bc18）。
// m_specs[0xfe] 由成员声明自动调用 HWSpec ctor（ORIG 循环 0x0822bc51..0x0822bc67）。
#include "Packet_Client_Spec_Statistic.h"

Packet_Client_Spec_Statistic::Packet_Client_Spec_Statistic()
    : PacketHeader(0x9ce, 0xc06)   // ORIG _ZN12PacketHeaderC1Ett
    , m_flagB(0)                   // movb $0x0,0xb(%eax)
{
}

// ============================================================================
// LinkFinal2 核销 —— HWSpec::HWSpec()（ORIG W @0x0822b890，0x1f）。
// objdump 逐指令转写：+0x00 byte=0xff；+0x04/+0x08 dword=0xffffffff。
// ============================================================================
HWSpec::HWSpec()
{
    m_category1 = static_cast<char>(0xff);   // ORIG movb $0xff,(%eax)
    m_category2 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x04
    m_category3 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x08
}
