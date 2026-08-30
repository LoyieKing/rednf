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

    // ORIG ctor 0x0822be8a：+0xa/+0xe 两个 dword 清零（2×movl 直接位移）。
    unsigned int m_guildKey;   // +0x0a
    unsigned int m_charNo;     // +0x0e
};

#pragma pack(pop)

static_assert(sizeof(Packet_Guild_Cargo_History_Request) == 0x12, "Packet_Guild_Cargo_History_Request layout");

#endif  // DNF_GAME_PACKET_Packet_Guild_Cargo_History_Request_H_
