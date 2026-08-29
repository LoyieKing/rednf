#ifndef DNF_GAME_PACKET_Packet_Approve_Join_Guild_H_
#define DNF_GAME_PACKET_Packet_Approve_Join_Guild_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822c342
// _ZN25Packet_Approve_Join_GuildC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x16。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Approve_Join_Guild : public PacketHeader
{
public:
    Packet_Approve_Join_Guild();   // ORIG W @0x0822c342

    char m_data[0x16 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Approve_Join_Guild) == 0x16, "Packet_Approve_Join_Guild layout");

#endif  // DNF_GAME_PACKET_Packet_Approve_Join_Guild_H_
