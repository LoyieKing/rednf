#ifndef DNF_GAME_PACKET_Packet_Stat_Compatibility_Index_H_
#define DNF_GAME_PACKET_Packet_Stat_Compatibility_Index_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822c61e
// _ZN31Packet_Stat_Compatibility_IndexC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x1e。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Stat_Compatibility_Index : public PacketHeader
{
public:
    Packet_Stat_Compatibility_Index();   // ORIG W @0x0822c61e

    char m_data[0x1e - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Stat_Compatibility_Index) == 0x1e, "Packet_Stat_Compatibility_Index layout");

#endif  // DNF_GAME_PACKET_Packet_Stat_Compatibility_Index_H_
