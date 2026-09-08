// ============================================================================
// LinkHelper 核销批次 —— Packet_Approve_Join_Guild ctor（ORIG W @0x0822c342 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Approve_Join_Guild.h"

Packet_Approve_Join_Guild::Packet_Approve_Join_Guild()
    : PacketHeader(0x1bc2, 0x16)   // ORIG _ZN12PacketHeaderC1Ett
{
    // ORIG 0x0822c342：三个独立 movl $0,0xa/0xe/0x12（非 memset 调用）。
    m_fields[0] = 0;   // +0x0a
    m_fields[1] = 0;   // +0x0e
    m_fields[2] = 0;   // +0x12
}
