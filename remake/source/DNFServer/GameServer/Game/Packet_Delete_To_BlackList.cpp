// ============================================================================
// LinkHelper 核销批次 —— Packet_Delete_To_BlackList ctor（ORIG W @0x0822bb70 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Delete_To_BlackList.h"

Packet_Delete_To_BlackList::Packet_Delete_To_BlackList()
    : PacketHeader(0x2c, 0x5dd)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0x4, 0, 0x1e);   // ORIG memset(this+0xe, 0, 0x1e)
}
