// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp
// Packet_Monitor_UDP_Reply_UserInfo：ORIG 弱内联 ctor W @0x082a3f52。
// has_def=1：完整复制定义自 PacketCtor_Classes.cpp:68（class : PacketHeader，
// PacketHeader(0x3ea,0x16e6)，packed；ctor 从 m_items(+0xc) 起零填充 0x16da 字节）。
// ctor 体仍由 PacketCtor_Classes.cpp 提供。
#ifndef DNF_GAME_PACKET_MONITOR_UDP_REPLY_USERINFO_H_
#define DNF_GAME_PACKET_MONITOR_UDP_REPLY_USERINFO_H_

#include "PacketHeader.h"

// ST_MonitorReplyUserInfo（ORIG 条目步长 0x4e；证据 = ORIG
// Send_userinfos_to_upper_server 0x082961be 段逐字段写偏移：每字段
// `imul $0x4e,%ebx,%edx` 步长 + item+0x0/+0x4/+0x8/+0xc 四 dword（0xc/0x10/
// 0x14/0x18 disp）、+0x10/+0x11 两字节（mov %al）、+0x12 word、+0x14 name、
// +0x32 memberKey、+0x36 ip、+0x46 sex、+0x47 ssn；packetSize=n*0x4e+0xc）。
// b28d-solo-CGameManager 修正：旧布局 m_job 为 4B 使 sizeof=0x52，与 ORIG 0x4e
// 差 4 ⇒ item 步长/后续字段偏移全错；现与 shared/packet/include 同名头一致，
// 消除 Game/Guild/Monitor 三方 ODR 分歧。
#pragma pack(push, 1)
struct ST_MonitorReplyUserInfo
{
    unsigned int m_dbid;        // +0x00
    unsigned int m_charNo;      // +0x04
    unsigned int m_channel;     // +0x08
    unsigned int m_guildKey;    // +0x0c
    unsigned char m_job;        // +0x10（ORIG `mov %al,0xc(%edx)` 1 字节写）
    unsigned char m_grade;      // +0x11
    unsigned short m_level;     // +0x12
    char m_name[0x1e];          // +0x14
    unsigned int m_memberKey;   // +0x32
    char m_ip[0x10];            // +0x36
    unsigned char m_sex;        // +0x46
    char m_ssn[7];              // +0x47（ORIG memcpy 6；尾字节对齐到 0x4e）
};
#pragma pack(pop)

class Packet_Monitor_UDP_Reply_UserInfo : public PacketHeader
{
public:
    Packet_Monitor_UDP_Reply_UserInfo();   // ORIG W @0x082a3f52（体：PacketCtor_Classes.cpp）
    unsigned char m_count;    // +0xa
    unsigned char m_channel;  // +0xb
    ST_MonitorReplyUserInfo m_items[75];  // +0xc（75 × 0x4e = 0x16da）
} __attribute__((packed));
#endif  // DNF_GAME_PACKET_MONITOR_UDP_REPLY_USERINFO_H_
