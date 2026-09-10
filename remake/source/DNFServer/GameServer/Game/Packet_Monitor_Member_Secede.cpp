// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Monitor_Member_Secede/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Packet_Monitor_Member_Secede.h"

#include "Packet_Monitor_Member_Secede/Packet_Monitor_Member_Secede.inc"
    // b28c-33 真语义 bug 修复：ORIG 0x0822ba34 为 `movl $0x31,0x8(%esp);
    // movl $0x4bb,0x4(%esp)` ⇒ 第 1 实参=packetId=0x4bb（dispatcher 值域
    // 0x3e8..0x27ff）、第 2 实参=packetSize=0x31；原写法两参颠倒。
    : PacketHeader(0x4bb, 0x31)
{
    memset(m_data + 0x9, 0, 0x1e);   // ORIG memset(this+0x13, 0, 0x1e)
}
