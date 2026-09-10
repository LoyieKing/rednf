// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Monitor_Request_Member_Enter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Packet_Monitor_Request_Member_Enter.h"

#include "Packet_Monitor_Request_Member_Enter/Packet_Monitor_Request_Member_Enter.inc"
    // [b28c-34] 真语义 bug 修复：ORIG 0x0822b9ce 段 `movl $0x30,0x8(%esp);
    // movl $0x4b7,0x4(%esp)` ⇒ PacketHeader(packetId=0x4b7, packetSize=0x30)；
    // 原写 (0x30,0x4b7) 使包 ID/长度互换。
    : PacketHeader(0x4b7, 0x30)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0x8, 0, 0x1e);   // ORIG memset(this+0x12, 0, 0x1e)
}
