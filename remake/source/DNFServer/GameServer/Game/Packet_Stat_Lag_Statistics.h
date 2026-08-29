#ifndef DNF_GAME_PACKET_STAT_LAG_STATISTICS_H_
#define DNF_GAME_PACKET_STAT_LAG_STATISTICS_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt：Packet_Stat_Lag_Statistics ctor/dtor）。
// ORIG 证据：C1 W @0x0822c144（PacketHeader(0x14c,0x17b9) + 7×stModuleLagInfo@+0xb
// stride 0xa + 9×stDunGeonLagInfo@+0x5c stride 0x18）；
// D1 W @0x082354ec（逆序调用子对象析构）。
// 子对象 ORIG：stModuleLagInfo C1 @0x0822c0c0 / D1 @0x0822c0e2（sz 0xa）；
//             stDunGeonLagInfo C1 @0x0822c0e8 / D1 @0x0822c13e（sz 0x18，packed）。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class stModuleLagInfo
{
public:
    stModuleLagInfo() : m_a(0), m_b(0), m_c(0) {}
    ~stModuleLagInfo() {}

    unsigned int   m_a;   // +0x00
    unsigned int   m_b;   // +0x04
    unsigned short m_c;   // +0x08
};                        // sizeof 0xa

class stDunGeonLagInfo
{
public:
    stDunGeonLagInfo()
        : m_w0(0), m_b2(0), m_i3(0), m_i7(0)
        , m_wb(0), m_bd(0), m_ie(0), m_i12(0), m_w16(0)
    {
    }
    ~stDunGeonLagInfo() {}

    unsigned short m_w0;    // +0x00
    char           m_b2;    // +0x02
    unsigned int   m_i3;    // +0x03
    unsigned int   m_i7;    // +0x07
    unsigned short m_wb;    // +0x0b
    char           m_bd;    // +0x0d
    unsigned int   m_ie;    // +0x0e
    unsigned int   m_i12;   // +0x12
    unsigned short m_w16;   // +0x16
};                          // sizeof 0x18

class Packet_Stat_Lag_Statistics : public PacketHeader
{
public:
    Packet_Stat_Lag_Statistics();   // ORIG W @0x0822c144
    ~Packet_Stat_Lag_Statistics();  // ORIG W @0x082354ec

    char              m_flagA;        // +0x0a（ctor 置 0）
    stModuleLagInfo   m_modules[7];   // +0x0b..+0x50
    char              m_padMid[0xa];  // +0x51..+0x5a（ORIG ctor 未写）
    char              m_flagB;        // +0x5b（ctor 置 0）
    stDunGeonLagInfo  m_dungeons[9];  // +0x5c..+0x133
    char              m_tail[0x18];   // +0x134..+0x14b
};

#pragma pack(pop)

static_assert(sizeof(Packet_Stat_Lag_Statistics) == 0x14c, "Packet_Stat_Lag_Statistics layout");
static_assert(sizeof(stModuleLagInfo) == 0xa, "stModuleLagInfo layout");
static_assert(sizeof(stDunGeonLagInfo) == 0x18, "stDunGeonLagInfo layout");

#endif  // DNF_GAME_PACKET_STAT_LAG_STATISTICS_H_
