// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventMailTypesC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "InGameEventScripts.h"   // ::Action_SendMail / Condition_LevelUp 权威布局

#include "EventMailTypesC2/Action_SendMail.inc"


#include "EventMailTypesC2/dtor_Action_SendMail.inc"


// ============ Action_SendMail C1ERKS_/aSERKS_（ORIG W 0x08987b48/0x0898a922） ============
// [b28d-Lane SymSrvParam] 原 forceInstantiateActionSendMailCtors 已删：nm 实证
// 同名 W（C1ERKS/C2ERKS/aSERKS）由 EventScriptsCompleteB2.cpp.o 的
// std::pair<XxxRule, Action_SendMail> 实例化点自然发射，链接期 COMDAT 合并，
// 本 TU 无独立发射职责 ⇒ 删除补发函数不产生 only_orig。

#include "EventMailTypesC2/Condition_LevelUp.inc"


#include "EventMailTypesC2/dtor_Condition_LevelUp.inc"


#include "EventMailTypesC2/Reward_Item.inc"


#include "EventMailTypesC2/dtor_Reward_Item.inc"

