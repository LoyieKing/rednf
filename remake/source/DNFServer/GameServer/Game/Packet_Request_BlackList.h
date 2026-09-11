#ifndef DNF_GAME_PACKET_Packet_Request_BlackList_H_
#define DNF_GAME_PACKET_Packet_Request_BlackList_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bbb2
// _ZN24Packet_Request_BlackListC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x12。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Request_BlackList : public PacketHeader
{
public:
    Packet_Request_BlackList();   // ORIG W @0x0822bbb2

    // [g02 2026-09-11 · Main 已批准 · 「具名成员」轴] ORIG `Dispatcher_RequestBlackList::process`
    //   0x081ce1xx：`call get_acc_id; mov %eax,-0x14(%ebp)`（pkt@-0x1e 的 **单条 disp 直存**
    //   = pkt+0x0a）/ `call GetUID; mov %eax,-0x10(%ebp)`（= pkt+0x0e）；
    //   我方旧形（TU 局部 packed 视图 + reinterpret_cast）⇒ 4.4 恒 `lea -0x1e(%ebp),%ebx;
    //   mov %eax,0xa(%ebx)` 两步（前任「视图取址 → 引用形」已证无效）。
    //   pack(1) 下两成员恰填满 0xa..0x11，sizeof 恒 0x12。
    unsigned int m_dbid;          // +0x0a（原 m_data[0..3]）
    unsigned int m_idByChannel;   // +0x0e（原 m_data[4..7]）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Request_BlackList) == 0x12, "Packet_Request_BlackList layout");

#endif  // DNF_GAME_PACKET_Packet_Request_BlackList_H_
