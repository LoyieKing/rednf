// ============================================================================
// LinkHelper 核销批次 —— Packet_Guild_Attendance_Info ctor（ORIG W @0x0822c384 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Guild_Attendance_Info.h"

Packet_Guild_Attendance_Info::Packet_Guild_Attendance_Info()
    : PacketHeader(0x1bc6, 0x13),   // ORIG _ZN12PacketHeaderC1Ett
      m_fieldA(0), m_fieldB(0), m_fieldC(0)
{
}
