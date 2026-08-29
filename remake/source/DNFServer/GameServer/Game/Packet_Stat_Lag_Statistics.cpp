// ============================================================================
// LinkHelper 核销批次 —— Packet_Stat_Lag_Statistics ctor/dtor（/tmp/undef_r21.txt）。
// ORIG：C1 W @0x0822c144 / D1 W @0x082354ec（objdump 转写）。
// ============================================================================

#include "Packet_Stat_Lag_Statistics.h"

Packet_Stat_Lag_Statistics::Packet_Stat_Lag_Statistics()
    : PacketHeader(0x14c, 0x17b9)   // ORIG PacketHeader(0x14c, 0x17b9)
    , m_flagA(0)                    // movb $0x0,0xa(%eax)
    , m_flagB(0)                    // movb $0x0,0x5b(%eax)
{
    // m_modules[7]/m_dungeons[9] 子对象由成员声明自动构造
    //（ORIG 循环调 stModuleLagInfo/stDunGeonLagInfo ctor）。
}

// ORIG D1：逆序析构 9×stDunGeonLagInfo、7×stModuleLagInfo（隐式成员析构等效）。
Packet_Stat_Lag_Statistics::~Packet_Stat_Lag_Statistics()
{
}
