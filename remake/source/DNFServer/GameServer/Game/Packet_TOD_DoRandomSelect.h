// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x084e954c (C1 W), nm-only.
// Packet_TOD_DoRandomSelect：ORIG 弱内联 ctor W @0x084e954c。
// ctor 反汇编：仅 PacketHeader(this, 0x2712, 0xa)，无附加字段初始化。
#ifndef DNF_GAME_PACKET_TOD_DORANDOMSELECT_H_
#define DNF_GAME_PACKET_TOD_DORANDOMSELECT_H_

#include "PacketHeader.h"

class Packet_TOD_DoRandomSelect : public PacketHeader
{
public:
    Packet_TOD_DoRandomSelect() : PacketHeader(0x2712, 0xa) {}
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_TOD_DORANDOMSELECT_H_
