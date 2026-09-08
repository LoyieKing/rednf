// ============================================================================
// LinkHelper 核销批次 —— Packet_Assert_Manager_Info ctor（ORIG W @0x0822bd18 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Assert_Manager_Info.h"

Packet_Assert_Manager_Info::Packet_Assert_Manager_Info()
    : PacketHeader(0xc41, 0x214)   // ORIG _ZN12PacketHeaderC1Ett（packetId=0xc41, size=0x214；
                                   // ORIG 0x0822bd21 先压 0x214 后压 0xc41 ⇒ 实参序 (0xc41,0x214)）
{
}
