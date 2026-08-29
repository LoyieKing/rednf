// ============================================================================
// LinkHelper 核销批次 —— Packet_Guild_Cargo_History_Request ctor（ORIG W @0x0822be8a objdump 逐指令转写）。
// ============================================================================
#include "Packet_Guild_Cargo_History_Request.h"

Packet_Guild_Cargo_History_Request::Packet_Guild_Cargo_History_Request()
    : PacketHeader(0x70b, 0x12)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0, 0, 0x8);   // ORIG memset(this+0xa, 0, 0x8)
}
