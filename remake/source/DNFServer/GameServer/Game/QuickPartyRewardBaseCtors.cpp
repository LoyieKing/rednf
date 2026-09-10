// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 QuickPartyRewardBaseCtors/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— QuickParty::IRewardType 基类 ctor 独立 TU。
// ORIG 0x0826dbee（W）：vptr + 引用存储，逐指令同 CQuickPartyRewardManager.cpp 原实现。
// 归属实证：基类 ctor 与派生类 ctor（IBasicReward 0x826dc06 等）同 TU 时，
// gcc4.4 unit-at-a-time 将派生 ctor outgoing 帧由 sub $0x18 塌缩为 sub $0x8，
// 与 ORIG 4.1 恒发 $0x18 相悖（NL4 p6/p7 探针族 + NL7 三处同类实证）；
// 定义落独立 TU（头声明不变）后派生 ctor 恢复 0x18 帧。

#include "CQuickPartyRewardManager.h"

namespace QuickParty
{

#include "QuickPartyRewardBaseCtors/IRewardType.inc"
    : m_data(data)
{
}

}  // namespace QuickParty
