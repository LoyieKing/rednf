// ============================================================================
// LinkHelper 核销批次 —— Packet_Request_BlackList ctor（ORIG W @0x0822bbb2 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Request_BlackList.h"

// [b28c-31] AE（真 bug 修复）：ORIG 0x0822bbbb 压栈序 = PacketHeader(0x5e0, 0x12)
// （id=0x5e0,size=0x12），原写 (0x12,0x5e0) 实参反 ⇒ 已交换（10=10）。
Packet_Request_BlackList::Packet_Request_BlackList()
    : PacketHeader(0x5e0, 0x12)   // ORIG _ZN12PacketHeaderC1Ett（0x0822bbbb 第二实参 0x12 先压 ⇒ (id=0x5e0,size=0x12)）
{
}
