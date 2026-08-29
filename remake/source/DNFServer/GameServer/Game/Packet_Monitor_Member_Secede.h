#ifndef DNF_GAME_PACKET_Packet_Monitor_Member_Secede_H_
#define DNF_GAME_PACKET_Packet_Monitor_Member_Secede_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822ba34
// _ZN28Packet_Monitor_Member_SecedeC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x31。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Monitor_Member_Secede : public PacketHeader
{
public:
    Packet_Monitor_Member_Secede();   // ORIG W @0x0822ba34

    char m_data[0x31 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Member_Secede) == 0x31, "Packet_Monitor_Member_Secede layout");

#endif  // DNF_GAME_PACKET_Packet_Monitor_Member_Secede_H_
