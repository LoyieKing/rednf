// ============================================================================
// LinkHelper 核销批次 —— Packet_Delete_To_BlackList ctor（ORIG W @0x0822bb70 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Delete_To_BlackList.h"

Packet_Delete_To_BlackList::Packet_Delete_To_BlackList()
    : PacketHeader(0x5dd, 0x2c)   // ORIG _ZN12PacketHeaderC1Ett（packetId=0x5dd, size=0x2c；
                                  // ORIG 0x0822bb79 先压 0x2c 后压 0x5dd ⇒ 实参序 (0x5dd,0x2c)）
{
    memset(m_name, 0, 0x1e);   // ORIG memset(this+0xe, 0, 0x1e)
}
