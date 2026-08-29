#ifndef DNF_GAME_PACKET_Packet_Delete_To_BlackList_H_
#define DNF_GAME_PACKET_Packet_Delete_To_BlackList_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bb70
// _ZN26Packet_Delete_To_BlackListC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x2c。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Delete_To_BlackList : public PacketHeader
{
public:
    Packet_Delete_To_BlackList();   // ORIG W @0x0822bb70

    char m_data[0x2c - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Delete_To_BlackList) == 0x2c, "Packet_Delete_To_BlackList layout");

#endif  // DNF_GAME_PACKET_Packet_Delete_To_BlackList_H_
