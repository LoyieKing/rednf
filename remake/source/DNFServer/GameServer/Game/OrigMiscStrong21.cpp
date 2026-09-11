// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OrigMiscStrong21/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"

#include "OrigMiscStrong21/getIllustIndex.inc"


// [Q6HeaderTypeAxis 2026-09-11 P342·杠杆A] 原 `#include "dtor_stOneBuff_t.inc"` + 空体
//   已删（用户空析构解除 ⇒ stOneBuff_t 恢复 trivial）；D1/D2 由 ScriptLeafA2.cpp:297
//   `vector<stOneBuff_t>` / :324 栈上对象这条 odr-use 链发射（W，ORIG W 0x08a04b54）。
//   删前本 TU 3/3/0/0/1，删后 2/3/0/0/0（identical −1 系定义迁出，非丢件）。
