// ============================================================================
// LinkHelper 核销批次 —— Packet_Monitor_Set_Sub_Guild_Master ctor（ORIG W @0x0822b978 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Monitor_Set_Sub_Guild_Master.h"

Packet_Monitor_Set_Sub_Guild_Master::Packet_Monitor_Set_Sub_Guild_Master()
    : PacketHeader(0x31, 0x430)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0, 0, 0x27);   // ORIG memset(this+0xa, 0, 0x27)
}
