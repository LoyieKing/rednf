// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Request_BlackList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Packet_Request_BlackList.h"

// [b28c-31] AE（真 bug 修复）：ORIG 0x0822bbbb 压栈序 = PacketHeader(0x5e0, 0x12)
// （id=0x5e0,size=0x12），原写 (0x12,0x5e0) 实参反 ⇒ 已交换（10=10）。
#include "Packet_Request_BlackList/Packet_Request_BlackList.inc"
    : PacketHeader(0x5e0, 0x12)   // ORIG _ZN12PacketHeaderC1Ett（0x0822bbbb 第二实参 0x12 先压 ⇒ (id=0x5e0,size=0x12)）
{
}
