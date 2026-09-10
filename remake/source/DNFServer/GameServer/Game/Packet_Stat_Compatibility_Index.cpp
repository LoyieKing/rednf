// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Stat_Compatibility_Index/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Packet_Stat_Compatibility_Index.h"

#include "Packet_Stat_Compatibility_Index/Packet_Stat_Compatibility_Index.inc"
    // ORIG W 0x0822c61e：PacketHeader(0x27e0,0x1e) 后逐字段 movl/movb/movw 置 0，
    // 置位顺序 = 成员声明顺序（+0xa accId → +0x1d osBit），故不得合并为 memset。
    : PacketHeader(0x27e0, 0x1e)   // ORIG _ZN12PacketHeaderC1Ett
{
    m_accId = 0;        // +0x0a movl
    m_cpuVendor = 0;    // +0x0e movb
    m_cpuNum = 0;       // +0x0f movb
    m_cpuClock = 0;     // +0x10 movl
    m_ram = 0;          // +0x14 movw
    m_videoVendor = 0;  // +0x16 movw
    m_videoDevice = 0;  // +0x18 movw
    m_videoRam = 0;     // +0x1a movw
    m_os = 0;           // +0x1c movb
    m_osBit = 0;        // +0x1d movb
}
