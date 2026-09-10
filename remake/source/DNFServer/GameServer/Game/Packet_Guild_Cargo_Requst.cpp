// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Guild_Cargo_Requst/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Packet_Guild_Cargo_Requst.h"

// ORIG 0x0822be73/0x0822be7d：this 重取后两条 movl $0x0,0xa(%eax)/0xe(%eax)
// ——具名字段赋值（勿用 memset，-O0 会外联 memset@plt，见 b23 字段化）。
// [b22判词·AE] 指令序列与 ORIG 逐行同（14=14）；--no-compile 读未 relink .o，
// 唯一残差 call PacketHeaderC1Ett 为重定位占位（call <本函数+off>），数据地址/
// 调用目标 relink 后转 AE；非语义差。
#include "Packet_Guild_Cargo_Requst/Packet_Guild_Cargo_Requst.inc"
