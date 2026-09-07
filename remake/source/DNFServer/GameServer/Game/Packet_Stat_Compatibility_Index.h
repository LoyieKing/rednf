#ifndef DNF_GAME_PACKET_Packet_Stat_Compatibility_Index_H_
#define DNF_GAME_PACKET_Packet_Stat_Compatibility_Index_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822c61e
// _ZN31Packet_Stat_Compatibility_IndexC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x1e。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Stat_Compatibility_Index : public PacketHeader
{
public:
    Packet_Stat_Compatibility_Index();   // ORIG W @0x0822c61e

    // 具名字段（ORIG ctor 0x0822c61e 逐字段 mov 宽度实证，与
    // shared/packet/include/Packet_Stat_Compatibility_Index.h 权威字段一致）：
    // movl→int / movb→uchar / movw→ushort，偏移 +0xa..+0x1d，共 0x14 字节。
    unsigned int  m_accId;        // +0x0a
    unsigned char m_cpuVendor;    // +0x0e
    unsigned char m_cpuNum;       // +0x0f
    int           m_cpuClock;     // +0x10
    unsigned short m_ram;         // +0x14
    unsigned short m_videoVendor; // +0x16
    unsigned short m_videoDevice; // +0x18
    unsigned short m_videoRam;    // +0x1a
    unsigned char m_os;           // +0x1c
    unsigned char m_osBit;        // +0x1d
};

#pragma pack(pop)

static_assert(sizeof(Packet_Stat_Compatibility_Index) == 0x1e, "Packet_Stat_Compatibility_Index layout");

#endif  // DNF_GAME_PACKET_Packet_Stat_Compatibility_Index_H_
