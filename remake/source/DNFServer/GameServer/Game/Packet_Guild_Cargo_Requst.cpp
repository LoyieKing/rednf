// ============================================================================
// LinkHelper 核销批次 —— Packet_Guild_Cargo_Requst ctor（ORIG W @0x0822be52 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Guild_Cargo_Requst.h"

// ORIG 0x0822be73/0x0822be7d：this 重取后两条 movl $0x0,0xa(%eax)/0xe(%eax)
// ——具名字段赋值（勿用 memset，-O0 会外联 memset@plt，见 b23 字段化）。
// [b22判词·AE] 指令序列与 ORIG 逐行同（14=14）；--no-compile 读未 relink .o，
// 唯一残差 call PacketHeaderC1Ett 为重定位占位（call <本函数+off>），数据地址/
// 调用目标 relink 后转 AE；非语义差。
Packet_Guild_Cargo_Requst::Packet_Guild_Cargo_Requst()
    : PacketHeader(0x70a, 0x12)   // ORIG _ZN12PacketHeaderC1Ett
{
    m_characNo = 0;
    m_field0e = 0;
}
