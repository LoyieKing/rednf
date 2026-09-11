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

    // [g02 2026-09-11 · Main 已批准 · P367 第 4 例] ORIG `Dispatcher_RequestMemberEnter::process`
    //   0x081cb90e `call GetUID; mov %eax,-0x36(%ebp)`（pkt@-0x40 ⇒ **+0x0a 单级直存**）
    //   0x081cb91c `call getCurCharacNo; mov %eax,-0x32(%ebp)`（**+0x0e 直存**）
    //   0x081cb933 `lea -0x40(%ebp),%eax; add $0x12,%eax`（名区 = **+0x12**）
    //   两处 store 均为**无转换的纯 mov** ⇒ 字段与 RHS 同型（GetUID 返 unsigned / getCurCharacNo 返 int）。
    //   我方旧形（局部 packed 视图 + reinterpret_cast）恒产 `lea -0x40(%ebp),%ebx; mov %eax,0xa(%ebx)` 两步。
    unsigned int m_uid;        // +0x0a（同型 ← GetUID）
    int m_characNo;            // +0x0e（同型 ← getCurCharacNo）
    char m_name[0x1e];         // +0x12（strncpy dst）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Request_Member_Enter) == 0x30, "Packet_Monitor_Request_Member_Enter layout");

#endif  // DNF_GAME_PACKET_Packet_Monitor_Request_Member_Enter_H_
