#include "Packet_Item_Limit_Edition_Buyable_Query.h"

// ORIG _ZN39Packet_Item_Limit_Edition_Buyable_QueryC1Ev / C2Ev @ 0x0822c064
// Weak(普通构造)：正文仅转发到 PacketHeader(0x1009, 0x1dc)，不初始化载荷字段。
Packet_Item_Limit_Edition_Buyable_Query::Packet_Item_Limit_Edition_Buyable_Query()
    : PacketHeader(0x1009, 0x1dc)
{
}
