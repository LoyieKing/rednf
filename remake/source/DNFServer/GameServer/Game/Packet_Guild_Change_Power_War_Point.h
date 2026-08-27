// Packet_Guild_Change_Power_War_Point —— 公会服务器 PowerWar 积分变更包。
// ORIG 唯一生命周期符号：W ctor 0x082f070c（0x74 字节，无 D1/C2——POD、
// 编译器弱空析构被消除）。布局证据 = ctor 反汇编：
//   PacketHeader::PacketHeader(this, 0x6a4, 0x2d)   （基类 +0x00..+0x09）
//   movb $0x0,+0xa        → m_powerSide
//   movb $0x0,+0x2b       → m_winPoint
//   movb $0x0,+0x2c       → m_losePoint
//   memset(this+0xb,0,16) / memset(this+0x1b,0,16) → m_characNo[8]
// 字段写入侧证据：pvp_assault::CAssaultPlace::_SendPowerWarPointToGuildServer
// @0x082eceb2（winSide→+0xa；胜者 characNo auStack_62[0..3]→+0xb 起步长 4；
// 败者→+0x1b 起；胜方 pp→+0x2b；败方 pp→+0x2c）。
#ifndef DNF_PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
#define DNF_PACKET_GUILD_CHANGE_POWER_WAR_POINT_H

#include "PacketHeader.h"    // 权威 PacketHeader（source/shared/packet/include）

class Packet_Guild_Change_Power_War_Point : public PacketHeader
{
public:
    // ORIG W 0x082f070c（内联展开由类定义产生弱符号）
    Packet_Guild_Change_Power_War_Point()
        : PacketHeader(0x6a4, 0x2d), m_powerSide(0), m_winPoint(0), m_losePoint(0)
    {
        memset(m_characNo, 0, sizeof(m_characNo));
    }

    char          m_powerSide;          // +0x0a
    unsigned int  m_characNo[8];        // +0x0b..+0x2a（[0..3] 胜者 / [4..7] 败者）
    unsigned char m_winPoint;           // +0x2b
    unsigned char m_losePoint;          // +0x2c
} __attribute__((packed));

#endif  // DNF_PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
