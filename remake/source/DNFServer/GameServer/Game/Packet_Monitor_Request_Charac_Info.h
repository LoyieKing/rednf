#ifndef DNF_GAME_PACKET_MONITOR_REQUEST_CHARAC_INFO_H_
#define DNF_GAME_PACKET_MONITOR_REQUEST_CHARAC_INFO_H_

// LinkHelper 核销批次：ORIG ctor W @0x0822c5d2 _ZN32Packet_Monitor_Request_Charac_InfoC1Ev
// objdump 转写：PacketHeader(0x23,0x27de) + memset(this+0xa,0,0x15) + *(uint*)(this+0x1f)=0。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Monitor_Request_Charac_Info : public PacketHeader
{
public:
    Packet_Monitor_Request_Charac_Info();   // ORIG W @0x0822c5d2

    char         m_data[0x15];   // +0x0a..+0x1e
    unsigned int m_field1f;      // +0x1f（ctor 置 0）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Request_Charac_Info) == 0x23, "layout");

#endif  // DNF_GAME_PACKET_MONITOR_REQUEST_CHARAC_INFO_H_
