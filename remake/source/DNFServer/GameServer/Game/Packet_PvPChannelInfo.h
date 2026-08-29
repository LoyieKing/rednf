#ifndef DNF_GAME_PACKET_Packet_PvPChannelInfo_H_
#define DNF_GAME_PACKET_Packet_PvPChannelInfo_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bd3c
// _ZN21Packet_PvPChannelInfoC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x17f8。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_PvPChannelInfo : public PacketHeader
{
public:
    Packet_PvPChannelInfo();   // ORIG W @0x0822bd3c

    char m_data[0x17f8 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_PvPChannelInfo) == 0x17f8, "Packet_PvPChannelInfo layout");

#endif  // DNF_GAME_PACKET_Packet_PvPChannelInfo_H_
