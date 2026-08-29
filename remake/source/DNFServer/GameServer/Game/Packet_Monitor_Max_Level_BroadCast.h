#ifndef DNF_GAME_PACKET_MONITOR_MAX_LEVEL_BROADCAST_H_
#define DNF_GAME_PACKET_MONITOR_MAX_LEVEL_BROADCAST_H_

// Packet_Monitor_Max_Level_BroadCast：ORIG 仅导出弱内联 ctor W @0x08694560。
// ctor 反汇编（0x08694560）：PacketHeader(this, 0x177e, 0x28)；
// memset(this+0xa, 0, 0x1e)。数据区即名字 @+0xa。
#include <string.h>

#include "PacketHeader.h"

class Packet_Monitor_Max_Level_BroadCast : public PacketHeader
{
public:
    Packet_Monitor_Max_Level_BroadCast()
        : PacketHeader(0x177e, 0x28)
    {
        memset(m_name, 0, sizeof(m_name));
    }

    char m_name[0x1e];  // +0xa
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_MONITOR_MAX_LEVEL_BROADCAST_H_
