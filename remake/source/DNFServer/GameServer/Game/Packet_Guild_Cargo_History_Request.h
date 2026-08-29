#ifndef DNF_GAME_PACKET_Packet_Guild_Cargo_History_Request_H_
#define DNF_GAME_PACKET_Packet_Guild_Cargo_History_Request_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822be8a
// _ZN34Packet_Guild_Cargo_History_RequestC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x12。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Guild_Cargo_History_Request : public PacketHeader
{
public:
    Packet_Guild_Cargo_History_Request();   // ORIG W @0x0822be8a

    char m_data[0x12 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Guild_Cargo_History_Request) == 0x12, "Packet_Guild_Cargo_History_Request layout");

#endif  // DNF_GAME_PACKET_Packet_Guild_Cargo_History_Request_H_
