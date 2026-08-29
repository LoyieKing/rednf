// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_Reason_Crash_Down_Info：ORIG 弱内联 ctor W @0x0861007e =
// PacketHeader(0xc48, 0x1a) + 4×int 清零（objdump 0861007e-086100c8：
// movl $0,+0xa/+0xe/+0x12/+0x16）；SendPacket(&packet,0x1a) 见
// docs/class_func_reports/CClientCrashDownLogStatistic/CarryClientCrashDown.md。
// 与 shared/packet/include/Packet_Reason_Crash_Down_Info.h 同构。
#ifndef DNF_GAME_PACKET_REASON_CRASH_DOWN_INFO_H_
#define DNF_GAME_PACKET_REASON_CRASH_DOWN_INFO_H_

#include "PacketHeader.h"

class Packet_Reason_Crash_Down_Info : public PacketHeader
{
public:
    Packet_Reason_Crash_Down_Info()   // ORIG W @0x0861007e
        : PacketHeader(0xc48, 0x1a), m_occTime(0), m_channel(0), m_reason(0), m_cnt(0)
    {
    }

    int m_occTime;   // +0x0a
    int m_channel;   // +0x0e
    int m_reason;    // +0x12
    int m_cnt;       // +0x16
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_REASON_CRASH_DOWN_INFO_H_
