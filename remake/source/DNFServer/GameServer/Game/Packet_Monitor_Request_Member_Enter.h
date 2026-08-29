#ifndef DNF_GAME_PACKET_Packet_Monitor_Request_Member_Enter_H_
#define DNF_GAME_PACKET_Packet_Monitor_Request_Member_Enter_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822b9ce
// _ZN35Packet_Monitor_Request_Member_EnterC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x30。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Monitor_Request_Member_Enter : public PacketHeader
{
public:
    Packet_Monitor_Request_Member_Enter();   // ORIG W @0x0822b9ce

    char m_data[0x30 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Request_Member_Enter) == 0x30, "Packet_Monitor_Request_Member_Enter layout");

#endif  // DNF_GAME_PACKET_Packet_Monitor_Request_Member_Enter_H_
