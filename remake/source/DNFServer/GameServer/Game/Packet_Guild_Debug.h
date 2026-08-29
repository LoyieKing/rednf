// ORIG 弱内联 ctor W _ZN17Packet_Guild_DebugC1Ev @0x084b41a8 (sz 0x55)：
//   PacketHeader(0x1bc9, 0x1e)
//   movl $0x0, 0xa/0xe/0x12/0x16/0x1a(this) —— 5 个 4 字节字段清零。
// 布局 0x1e = 0x0a + 5*4；GmCmds_C.cpp CGuildDebug::execute 消费
// （m_mode 写 int 值，故为 4 字节 unsigned int，非 char）。
#ifndef DNF_GAME_PACKET_GUILD_DEBUG_H_
#define DNF_GAME_PACKET_GUILD_DEBUG_H_

#include "PacketHeader.h"

class Packet_Guild_Debug : public PacketHeader
{
public:
    Packet_Guild_Debug()
        : PacketHeader(0x1bc9, 0x1e)
    {
        m_charNo = 0;
        m_guildKey = 0;
        m_mode = 0;
        m_field16 = 0;
        m_field1a = 0;
    }
    unsigned int m_charNo;       // +0x0a
    unsigned int m_guildKey;     // +0x0e
    unsigned int m_mode;         // +0x12
    unsigned int m_field16;      // +0x16
    unsigned int m_field1a;      // +0x1a
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Debug, 0x1e)
#endif  // DNF_GAME_PACKET_GUILD_DEBUG_H_
