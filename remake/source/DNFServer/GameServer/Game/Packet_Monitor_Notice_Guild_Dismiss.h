#ifndef DNF_GAME_PACKET_MONITOR_NOTICE_GUILD_DISMISS_H_
#define DNF_GAME_PACKET_MONITOR_NOTICE_GUILD_DISMISS_H_

// Packet_Monitor_Notice_Guild_Dismiss：ORIG 弱内联 ctor W @0x084b4156。
// ctor 反汇编（0x084b4156）：PacketHeader(this, 0x3fc, 0xe)；
// movl $0,(this+0xa) —— 公会键 4 字节清零 @+0xa。
#include "PacketHeader.h"

class Packet_Monitor_Notice_Guild_Dismiss : public PacketHeader
{
public:
    Packet_Monitor_Notice_Guild_Dismiss()
        : PacketHeader(0x3fc, 0xe), m_guildKey(0)
    {
    }

    unsigned int m_guildKey;  // +0xa
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_MONITOR_NOTICE_GUILD_DISMISS_H_
