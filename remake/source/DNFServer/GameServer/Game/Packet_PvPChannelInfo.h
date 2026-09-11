#ifndef DNF_GAME_PACKET_Packet_PvPChannelInfo_H_
#define DNF_GAME_PACKET_Packet_PvPChannelInfo_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822bd3c
// _ZN21Packet_PvPChannelInfoC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x17f8。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_PvPChannelInfo : public PacketHeader
{
public:
    Packet_PvPChannelInfo();   // ORIG W @0x0822bd3c

    // [G00 2026-09-11] ORIG 0x0821796x..0x08217ab9 段对 channelInfo 的四处载荷写全是
    //   `mov %eax,-0x17fe(%ebp)`（+0xe）/`mov %eax,-0x1802(%ebp)`（+0xa）/
    //   `movb $0x9,-0x17f6(%ebp)`（+0x16）/`mov %eax,-0x17fa(%ebp)`（+0x12）
    //   ⇒ 单条 ebp 相对折叠 = 真实具名成员（cast 视图指针恒生 lea+off 两步）。
    unsigned int m_characNo;   // +0x0a
    unsigned int m_uid;        // +0x0e
    unsigned int m_schoolNo;   // +0x12
    unsigned char m_field16;   // +0x16
    char m_data[0x17f8 - 0xa - 0xd];   // +0x17 起（尾部占位，sizeof 不变）
};

#pragma pack(pop)

static_assert(sizeof(Packet_PvPChannelInfo) == 0x17f8, "Packet_PvPChannelInfo layout");

#endif  // DNF_GAME_PACKET_Packet_PvPChannelInfo_H_
