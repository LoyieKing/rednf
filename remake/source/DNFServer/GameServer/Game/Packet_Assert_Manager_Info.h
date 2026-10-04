#ifndef DNF_GAME_PACKET_Packet_Assert_Manager_Info_H_
#define DNF_GAME_PACKET_Packet_Assert_Manager_Info_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bd18
// _ZN26Packet_Assert_Manager_InfoC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x214。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Assert_Manager_Info : public PacketHeader
{
public:
    Packet_Assert_Manager_Info();   // ORIG W @0x0822bd18

    int m_category;             // +0x0a
    char m_desc[0x100];           // +0x0e
    unsigned short m_f10e;        // +0x10e
    int m_f110;                   // +0x110
    char m_detail[0x100];         // +0x114
};

#pragma pack(pop)

static_assert(sizeof(Packet_Assert_Manager_Info) == 0x214, "Packet_Assert_Manager_Info layout");

#endif  // DNF_GAME_PACKET_Packet_Assert_Manager_Info_H_
