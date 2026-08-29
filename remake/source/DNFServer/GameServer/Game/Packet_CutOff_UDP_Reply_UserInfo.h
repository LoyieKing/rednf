#ifndef DNF_GAME_PACKET_CUTOFF_UDP_REPLY_USERINFO_H_
#define DNF_GAME_PACKET_CUTOFF_UDP_REPLY_USERINFO_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp@27（源码定义复制）
// ORIG W ctor _ZN30Packet_CutOff_UDP_Reply_UserInfoC2Ev 0x082a3fa2。

#include "PacketHeader.h"
#include <string.h>

// 类声明与布局复制自 PacketCtor_Classes.cpp:27-40
//（ctor 语义 PacketHeader(0x3f9,0xafe)；m_count=0、m_channel=255、m_group=0、m_accIds 全零）。
class Packet_CutOff_UDP_Reply_UserInfo : public PacketHeader
{
public:
    Packet_CutOff_UDP_Reply_UserInfo();
    unsigned short m_count;               // +0xa
    unsigned char m_channel;              // +0xc
    unsigned char m_group;                // +0xd
    unsigned int m_accIds[0x2bc];         // +0xe
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_CUTOFF_UDP_REPLY_USERINFO_H_
