// ============================================================================
// LinkHelper 核销批次 —— Packet_Monitor_Member_Secede ctor（ORIG W @0x0822ba34 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Monitor_Member_Secede.h"

Packet_Monitor_Member_Secede::Packet_Monitor_Member_Secede()
    : PacketHeader(0x31, 0x4bb)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0x9, 0, 0x1e);   // ORIG memset(this+0x13, 0, 0x1e)
}
