#ifndef DNF_GAME_PACKET_Packet_Overflow_Statistic_Add_H_
#define DNF_GAME_PACKET_Packet_Overflow_Statistic_Add_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bdf0
// _ZN29Packet_Overflow_Statistic_AddC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0xd。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Overflow_Statistic_Add : public PacketHeader
{
public:
    Packet_Overflow_Statistic_Add();   // ORIG W @0x0822bdf0

    unsigned char m_kind;    // +0x0a
    unsigned short m_val;    // +0x0b
};

#pragma pack(pop)

static_assert(sizeof(Packet_Overflow_Statistic_Add) == 0xd, "Packet_Overflow_Statistic_Add layout");

#endif  // DNF_GAME_PACKET_Packet_Overflow_Statistic_Add_H_
