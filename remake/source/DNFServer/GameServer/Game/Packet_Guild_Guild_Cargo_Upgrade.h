// ORIG 弱内联 ctor W _ZN29Packet_Guild_Guild_Cargo_UpgradeC1Ev @0x0822bfea (sz 0x55)：
//   PacketHeader(0x0713, 0x1e)
//   movl $0x0, 0xa/0xe/0x12/0x16/0x1a(this) —— 5 个 4 字节字段清零。
// 布局 0x1e = 0x0a + 5*4（packetSize=0x14）；第 5 字段 +0x1a 由 ctor 清零，
//   消费 TU 未写入。DisPatcher_DebugCommand_cmds.cpp 消费。
#ifndef DNF_GAME_PACKET_GUILD_GUILD_CARGO_UPGRADE_H_
#define DNF_GAME_PACKET_GUILD_GUILD_CARGO_UPGRADE_H_

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Upgrade : public PacketHeader
{
public:
    Packet_Guild_Guild_Cargo_Upgrade()
        : PacketHeader(0x0713, 0x1e)
    {
        m_guildKey = 0;
        m_charNo = 0;
        m_curCapacity = 0;
        m_newCapacity = 0;
        m_field1a = 0;
    }
    unsigned int m_guildKey;         // +0x0a
    unsigned int m_charNo;           // +0x0e
    int m_curCapacity;               // +0x12
    int m_newCapacity;               // +0x16
    unsigned int m_field1a;          // +0x1a（ORIG ctor movl $0x0，无消费写入证据）
    static const unsigned int packetSize = 0x14;
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Guild_Cargo_Upgrade, 0x1e)
#endif  // DNF_GAME_PACKET_GUILD_GUILD_CARGO_UPGRADE_H_
