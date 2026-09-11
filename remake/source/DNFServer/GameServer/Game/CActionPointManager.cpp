#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CActionPointManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ APSystem::CActionPointManager 实现（批3：从 GameStubs.cpp 迁移）。
// 类声明位于 CUser.h（APSystem::CActionPointManager）。本 TU 是这些符号的唯一权威定义。
// ORIG：GM_Set @0x8121b3e（T）、Reset @0x8122af4（W）、GetActionPoint @0x8694bfc（W）。
#include "CUser.h"
#include <string.h>

namespace APSystem
{

// _ActionGroupIndex / CUserProc 权威声明见 CUserProc.h（本 TU 本地镜像已删）。

#include "CActionPointManager/GetActionPoint.inc"


#include "CActionPointManager/GM_Set.inc"


#include "CActionPointManager/Reset.inc"


} // namespace APSystem
