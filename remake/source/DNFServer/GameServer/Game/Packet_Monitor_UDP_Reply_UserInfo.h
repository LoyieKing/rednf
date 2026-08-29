// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp
// Packet_Monitor_UDP_Reply_UserInfo：ORIG 弱内联 ctor W @0x082a3f52。
// has_def=1：完整复制定义自 PacketCtor_Classes.cpp:68（class : PacketHeader，
// PacketHeader(0x3ea,0x16e6)，packed；ctor 从 m_items(+0xc) 起零填充 0x16da 字节）。
// ctor 体仍由 PacketCtor_Classes.cpp 提供。
#ifndef DNF_GAME_PACKET_MONITOR_UDP_REPLY_USERINFO_H_
#define DNF_GAME_PACKET_MONITOR_UDP_REPLY_USERINFO_H_

#include "PacketHeader.h"

// ST_MonitorReplyUserInfo（0x4d/条目；CGameManager.cpp 消费字段证据）
#pragma pack(push, 1)
struct ST_MonitorReplyUserInfo
{
    unsigned int m_dbid;
    unsigned int m_charNo;
    unsigned int m_channel;
    unsigned int m_guildKey;
    unsigned int m_job;
    unsigned char m_grade;
    unsigned short m_level;
    unsigned int m_memberKey;
    char m_name[0x1e];
    char m_ip[0x10];
    unsigned char m_sex;
    char m_ssn[6];
    char m_pad4b[2];
};
#pragma pack(pop)

class Packet_Monitor_UDP_Reply_UserInfo : public PacketHeader
{
public:
    Packet_Monitor_UDP_Reply_UserInfo();   // ORIG W @0x082a3f52（体：PacketCtor_Classes.cpp）
    unsigned char m_count;    // +0xa
    unsigned char m_channel;  // +0xb
    ST_MonitorReplyUserInfo m_items[75];  // +0xc（75 × 0x4d）
} __attribute__((packed));
#endif  // DNF_GAME_PACKET_MONITOR_UDP_REPLY_USERINFO_H_
