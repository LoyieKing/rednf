#ifndef DNF_GAME_PACKET_Packet_Monitor_Set_Sub_Guild_Master_H_
#define DNF_GAME_PACKET_Packet_Monitor_Set_Sub_Guild_Master_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822b978
// _ZN35Packet_Monitor_Set_Sub_Guild_MasterC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x31。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Monitor_Set_Sub_Guild_Master : public PacketHeader
{
public:
    Packet_Monitor_Set_Sub_Guild_Master();   // ORIG W @0x0822b978

    char m_data[0x31 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Set_Sub_Guild_Master) == 0x31, "Packet_Monitor_Set_Sub_Guild_Master layout");

#endif  // DNF_GAME_PACKET_Packet_Monitor_Set_Sub_Guild_Master_H_
