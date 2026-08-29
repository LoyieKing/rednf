// ============================================================================
// LinkHelper 核销批次 —— Packet_Approve_Join_Guild ctor（ORIG W @0x0822c342 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Approve_Join_Guild.h"

Packet_Approve_Join_Guild::Packet_Approve_Join_Guild()
    : PacketHeader(0x16, 0x1bc2)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0, 0, 0xc);   // ORIG memset(this+0xa, 0, 0xc)
}
