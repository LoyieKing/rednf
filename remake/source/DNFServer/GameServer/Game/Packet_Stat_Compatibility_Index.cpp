// ============================================================================
// LinkHelper 核销批次 —— Packet_Stat_Compatibility_Index ctor（ORIG W @0x0822c61e objdump 逐指令转写）。
// ============================================================================
#include "Packet_Stat_Compatibility_Index.h"

Packet_Stat_Compatibility_Index::Packet_Stat_Compatibility_Index()
    : PacketHeader(0x27e0, 0x1e)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0, 0, 0x14);   // ORIG memset(this+0xa, 0, 0x14)
}
