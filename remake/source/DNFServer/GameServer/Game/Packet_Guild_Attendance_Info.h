#ifndef DNF_GAME_PACKET_Packet_Guild_Attendance_Info_H_
#define DNF_GAME_PACKET_Packet_Guild_Attendance_Info_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822c384
// _ZN28Packet_Guild_Attendance_InfoC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_fieldA/m_fieldB/m_fieldC，sizeof=0x13。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Guild_Attendance_Info : public PacketHeader
{
public:
    Packet_Guild_Attendance_Info();   // ORIG W @0x0822c384

    unsigned int m_fieldA;     // +0x0a（ORIG ctor movl 清零）
    unsigned int m_fieldB;     // +0x0e（ORIG ctor movl 清零）
    unsigned char m_fieldC;    // +0x12（ORIG ctor movb 清零）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Guild_Attendance_Info) == 0x13, "Packet_Guild_Attendance_Info layout");

#endif  // DNF_GAME_PACKET_Packet_Guild_Attendance_Info_H_
