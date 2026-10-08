// 布局依据 InsertValueStatistic 反汇编（ORIG 0x0860d8a4 实证）：
//   ctor 调 PacketHeader(0x17ca, 0x86) 后 movl $0x0,+0xa（m_level）；
//   memcpy 目标 +0xe（m_values）、SendPacket 0x86 → sizeof 0x86、pack(1)。
#ifndef DNF_GAME_PACKET_VALUE_STATISTIC_H_
#define DNF_GAME_PACKET_VALUE_STATISTIC_H_

#include "PacketHeader.h"

#pragma pack(push, 1)
struct Packet_Value_Statistic
{
    Packet_Value_Statistic() : m_header(0x17ca, 0x86), m_level(0) {}

    PacketHeader m_header;   // +0x00 包头(10 字节：id/size/reserved/connNo)
    int m_level;             // +0x0a
    int m_values[30];        // +0x0e（0x78 字节）
};  // sizeof == 0x86
#pragma pack(pop)

#endif  // DNF_GAME_PACKET_VALUE_STATISTIC_H_
