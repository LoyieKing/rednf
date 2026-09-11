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

    // [g02 2026-09-11 · Main 已批准] ORIG `Dispatcher_MemberSecede::process`
    //   0x081cbcc0/0x081cbce3 对 +0x0a/+0x0e 是**单级 ebp 直存**（`mov %eax,-0x37(%ebp)` /
    //   `mov %eax,-0x33(%ebp)` = pkt+0x0a / pkt+0x0e）⇒ 真源码为**同类型具名成员**；
    //   旧形（局部视图 + `reinterpret_cast`）4.4 恒产 `lea -0x41(%ebp),%ebx; mov %eax,0xa(%ebx)` 两步。
    //   pack(1) 下 int@+0x0a 非对齐，与 ORIG 直存形态一致；sizeof 仍 0x31。
    int m_uid;                 // +0x0a
    int m_characNo;            // +0x0e
    char m_data[0x31 - 0x12];  // +0x12 起（+0x12 pad；名区自 +0x13）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Member_Secede) == 0x31, "Packet_Monitor_Member_Secede layout");

#endif  // DNF_GAME_PACKET_Packet_Monitor_Member_Secede_H_
