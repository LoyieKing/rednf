// ============================================================================
// LinkHelper 核销批次 —— Packet_Monitor_Request_Member_Enter ctor（ORIG W @0x0822b9ce objdump 逐指令转写）。
// ============================================================================
#include "Packet_Monitor_Request_Member_Enter.h"

Packet_Monitor_Request_Member_Enter::Packet_Monitor_Request_Member_Enter()
    : PacketHeader(0x30, 0x4b7)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0x8, 0, 0x1e);   // ORIG memset(this+0x12, 0, 0x1e)
}
