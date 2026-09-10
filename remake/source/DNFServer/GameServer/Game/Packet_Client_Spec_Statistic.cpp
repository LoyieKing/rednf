// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Client_Spec_Statistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// LinkHelper 核销批次 —— Packet_Client_Spec_Statistic ctor（ORIG W @0x0822bc18）。
// m_specs[0xfe] 由成员声明自动调用 HWSpec ctor（ORIG 循环 0x0822bc51..0x0822bc67）。
#include "Packet_Client_Spec_Statistic.h"

#include "Packet_Client_Spec_Statistic/Packet_Client_Spec_Statistic.inc"
    : PacketHeader(0x9ce, 0xc06)   // ORIG _ZN12PacketHeaderC1Ett
    , m_flagB(0)                   // movb $0x0,0xb(%eax)
{
}

// ============================================================================
// LinkFinal2 核销 —— HWSpec::HWSpec()（ORIG W @0x0822b890，0x1f）。
// objdump 逐指令转写：+0x00 byte=0xff；+0x04/+0x08 dword=0xffffffff。
// ============================================================================
#include "Packet_Client_Spec_Statistic/HWSpec.inc"
{
    m_category1 = static_cast<char>(0xff);   // ORIG movb $0xff,(%eax)
    m_category2 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x04
    m_category3 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x08
}
