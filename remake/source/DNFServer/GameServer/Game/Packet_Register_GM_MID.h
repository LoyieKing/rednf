// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_Register_GM_MID：ORIG 弱内联 ctor W @0x086945d4。
// ctor 反汇编：PacketHeader(this, 0x1f44, 0xe)；movl $0,(this+0xa)。
#ifndef DNF_GAME_PACKET_REGISTER_GM_MID_H_
#define DNF_GAME_PACKET_REGISTER_GM_MID_H_

#include "PacketHeader.h"

class Packet_Register_GM_MID : public PacketHeader
{
public:
    Packet_Register_GM_MID()
        : PacketHeader(0x1f44, 0xe), m_field0a(0)
    {
    }

    int m_field0a;  // +0xa（ORIG 4 字节 movl 清零）
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_REGISTER_GM_MID_H_
