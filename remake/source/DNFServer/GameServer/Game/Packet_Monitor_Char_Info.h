// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp
// Packet_Monitor_Char_Info：ORIG 弱内联 ctor W @0x082a3ffa。
// has_def=1：完整复制定义自 PacketCtor_Classes.cpp:43（class : PacketHeader，
// PacketHeader(0x3ef,0x3f)，packed；ctor 逐字段初始化并 memset m_name）。ctor 体仍由
// PacketCtor_Classes.cpp 提供。
#ifndef DNF_GAME_PACKET_MONITOR_CHAR_INFO_H_
#define DNF_GAME_PACKET_MONITOR_CHAR_INFO_H_

#include "PacketHeader.h"

class Packet_Monitor_Char_Info : public PacketHeader
{
public:
    Packet_Monitor_Char_Info();   // ORIG W @0x082a3ffa（体：PacketCtor_Classes.cpp）
    unsigned int m_dbid;        // +0xa
    unsigned char m_channel;    // +0xe
    unsigned int m_charNo;      // +0xf
    unsigned int m_guildKey;    // +0x13
    char m_job;                 // +0x17
    char m_grade;               // +0x18
    short m_level;              // +0x19
    unsigned int m_memberKey;   // +0x1b
    char m_name[0x1e];          // +0x1f
    unsigned char m_channelType; // +0x3d
    char m_returnUser;          // +0x3e
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_MONITOR_CHAR_INFO_H_
