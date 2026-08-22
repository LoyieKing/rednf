// df_game_r Game/ 迁移 TU —— 8 个 Packet_* 包的默认构造（2026-08-21）。
// 从 GameStubs.cpp 拆出。类声明与布局权威在 shared/packet/include（各 Packet_*.h），
// 本 TU 以非 inline 定义提供强 T 构造符号（C1/C2），GCC 自动为 out-of-line ctor
// 生成 C1/C2 同址别名（与 ORIG 弱 W 别名一致）。ctor 语义逐字节对照 ORIG：
//   Packet_ChannelType                  0x082a4088
//   Packet_CutOff_UDP_Reply_UserInfo    0x082a3fa2
//   Packet_Monitor_Char_Info            0x082a3ffa
//   Packet_Monitor_UDP_Reply_UserInfo   0x082a3f52
//   Packet_Server_Match_data            0x085df954
//   Packet_DoubleCheck_UDP_Login        0x0846d58a
//   Packet_DoubleCheck_UDP_Logout       0x0846d5c6
//   Packet_Double_Check_Server_UDP_HeartBeat 0x0846d602
#include "PacketHeader.h"
#include <string.h>

// ---- Packet_ChannelType ----
class Packet_ChannelType : public PacketHeader
{
public:
    Packet_ChannelType();
    unsigned int m_channel;  // +0xa
    int m_type;              // +0xe
} __attribute__((packed));
Packet_ChannelType::Packet_ChannelType()
    : PacketHeader(0xfa4, 0x12) {}
// ---- Packet_CutOff_UDP_Reply_UserInfo ----
class Packet_CutOff_UDP_Reply_UserInfo : public PacketHeader
{
public:
    Packet_CutOff_UDP_Reply_UserInfo();
    unsigned short m_count;               // +0xa
    unsigned char m_channel;              // +0xc
    unsigned char m_group;                // +0xd
    unsigned int m_accIds[0x2bc];         // +0xe
} __attribute__((packed));
Packet_CutOff_UDP_Reply_UserInfo::Packet_CutOff_UDP_Reply_UserInfo()
    : PacketHeader(0x3f9, 0xafe), m_count(0), m_channel(255), m_group(0)
{
    memset(m_accIds, 0, sizeof(m_accIds));
}

// ---- Packet_Monitor_Char_Info ----
class Packet_Monitor_Char_Info : public PacketHeader
{
public:
    Packet_Monitor_Char_Info();
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
Packet_Monitor_Char_Info::Packet_Monitor_Char_Info()
    : PacketHeader(0x3ef, 0x3f), m_dbid(0), m_channel(255), m_charNo(0),
      m_guildKey(0), m_job(255), m_grade(255), m_level(65535), m_memberKey(0),
      m_returnUser(0)
{
    memset(m_name, 0, sizeof(m_name));
}

// ---- Packet_Monitor_UDP_Reply_UserInfo ----
class Packet_Monitor_UDP_Reply_UserInfo : public PacketHeader
{
public:
    Packet_Monitor_UDP_Reply_UserInfo();
    unsigned char m_count;    // +0xa
    unsigned char m_channel;  // +0xb
    char m_items[75 * 0x4d];  // +0xc（ST_MonitorReplyUserInfo 各 0x4d 字节，仅需外层零填充）
} __attribute__((packed));
Packet_Monitor_UDP_Reply_UserInfo::Packet_Monitor_UDP_Reply_UserInfo()
    : PacketHeader(0x3ea, 0x16e6), m_count(0), m_channel(255)
{
    memset((char*)this + 0xc, 0, 0x16da);
}

// ---- Packet_Server_Match_data ----
class Packet_Server_Match_data : public PacketHeader
{
public:
    Packet_Server_Match_data();
    char m_serverGroup;  // +0xa
    char m_win;          // +0xb
    char m_lose;         // +0xc
} __attribute__((packed));
Packet_Server_Match_data::Packet_Server_Match_data()
    : PacketHeader(0x1b67, 0xd), m_serverGroup(0), m_win(0), m_lose(0) {}
// ---- Packet_DoubleCheck_UDP_Login ----
class Packet_DoubleCheck_UDP_Login : public PacketHeader
{
public:
    Packet_DoubleCheck_UDP_Login();
    unsigned int m_userID;   // +0xa
    unsigned char m_channel; // +0xe
    unsigned char m_group;   // +0xf
} __attribute__((packed));
Packet_DoubleCheck_UDP_Login::Packet_DoubleCheck_UDP_Login()
    : PacketHeader(0xbb8, 0x10), m_userID(0), m_channel(255), m_group(255) {}

// ---- Packet_DoubleCheck_UDP_Logout ----
class Packet_DoubleCheck_UDP_Logout : public PacketHeader
{
public:
    Packet_DoubleCheck_UDP_Logout();
    int m_accId;             // +0xa
    unsigned char m_channel; // +0xe
    unsigned char m_serverGroup; // +0xf
} __attribute__((packed));
Packet_DoubleCheck_UDP_Logout::Packet_DoubleCheck_UDP_Logout()
    : PacketHeader(0xbb9, 0x10), m_accId(0), m_channel(255), m_serverGroup(255) {}

// ---- Packet_Double_Check_Server_UDP_HeartBeat ----
class Packet_Double_Check_Server_UDP_HeartBeat : public PacketHeader
{
public:
    Packet_Double_Check_Server_UDP_HeartBeat();
    unsigned char m_channel;  // +0xa
    unsigned char m_group;    // +0xb
} __attribute__((packed));
Packet_Double_Check_Server_UDP_HeartBeat::Packet_Double_Check_Server_UDP_HeartBeat()
    : PacketHeader(0xbba, 0xc), m_channel(255) {}