#ifndef DNF_GAME_PACKET_Packet_Guild_Cargo_Requst_H_
#define DNF_GAME_PACKET_Packet_Guild_Cargo_Requst_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822be52
// _ZN25Packet_Guild_Cargo_RequstC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x12。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Guild_Cargo_Requst : public PacketHeader
{
public:
    Packet_Guild_Cargo_Requst();   // ORIG W @0x0822be52

    char m_data[0x12 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Guild_Cargo_Requst) == 0x12, "Packet_Guild_Cargo_Requst layout");

#endif  // DNF_GAME_PACKET_Packet_Guild_Cargo_Requst_H_
