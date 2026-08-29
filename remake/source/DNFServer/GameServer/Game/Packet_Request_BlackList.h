#ifndef DNF_GAME_PACKET_Packet_Request_BlackList_H_
#define DNF_GAME_PACKET_Packet_Request_BlackList_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bbb2
// _ZN24Packet_Request_BlackListC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x12。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Request_BlackList : public PacketHeader
{
public:
    Packet_Request_BlackList();   // ORIG W @0x0822bbb2

    char m_data[0x12 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Request_BlackList) == 0x12, "Packet_Request_BlackList layout");

#endif  // DNF_GAME_PACKET_Packet_Request_BlackList_H_
