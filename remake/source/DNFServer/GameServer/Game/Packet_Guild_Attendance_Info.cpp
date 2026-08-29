// ============================================================================
// LinkHelper 核销批次 —— Packet_Guild_Attendance_Info ctor（ORIG W @0x0822c384 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Guild_Attendance_Info.h"

Packet_Guild_Attendance_Info::Packet_Guild_Attendance_Info()
    : PacketHeader(0x13, 0x1bc6)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0, 0, 0x9);   // ORIG memset(this+0xa, 0, 0x9)
}
