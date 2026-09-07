#ifndef DNF_GAME_PACKET_Packet_Delete_To_BlackList_H_
#define DNF_GAME_PACKET_Packet_Delete_To_BlackList_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bb70
// _ZN26Packet_Delete_To_BlackListC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + 具名字段，sizeof=0x2c。
// [b23S3C] m_data 拆字段：Dispatchers.cpp ORIG 0x081cdf0a 实证 accId→+0x0a
// （`mov %eax,-0x32(%ebp)`）、name→+0x0e（ORIG C1 memset(this+0xe,0,0x1e)）。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Delete_To_BlackList : public PacketHeader
{
public:
    Packet_Delete_To_BlackList();   // ORIG W @0x0822bb70

    unsigned int m_accId;      // +0x0a
    char m_name[0x1e];         // +0x0e（角色名区，ctor memset）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Delete_To_BlackList) == 0x2c, "Packet_Delete_To_BlackList layout");

#endif  // DNF_GAME_PACKET_Packet_Delete_To_BlackList_H_
