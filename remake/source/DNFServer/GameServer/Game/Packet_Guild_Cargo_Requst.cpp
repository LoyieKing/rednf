// ============================================================================
// LinkHelper 核销批次 —— Packet_Guild_Cargo_Requst ctor（ORIG W @0x0822be52 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Guild_Cargo_Requst.h"

Packet_Guild_Cargo_Requst::Packet_Guild_Cargo_Requst()
    : PacketHeader(0x70a, 0x12)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0, 0, 0x8);   // ORIG memset(this+0xa, 0, 0x8)
}
