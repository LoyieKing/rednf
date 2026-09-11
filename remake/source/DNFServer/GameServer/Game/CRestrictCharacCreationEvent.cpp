// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CRestrictCharacCreationEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 限制创建角色事件 CRestrictCharacCreationEvent 实现。
// 报告：docs/class_func_reports/CRestrictCharacCreationEvent/ 全部 7 个方法
// （CRestrictCharacCreationEvent / ~CRestrictCharacCreationEvent /
//  ~CRestrictCharacCreationEvent_08273c9a / StartEvent / StartEvent_08273d0a /
//  EndEvent / getLimitCreateCharacter）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN28CRestrictCharacCreationEventC2Ev`（ORIG 0x8273e2a）；ctor 已是首强符号。
#include <iostream>

#include "CRestrictCharacCreationEvent.h"

#include "GameTypes.h"             // cMyTrace

#include "CRestrictCharacCreationEvent/CRestrictCharacCreationEvent.inc"


#include "CRestrictCharacCreationEvent/dtor_CRestrictCharacCreationEvent.inc"


#include "CRestrictCharacCreationEvent/StartEvent.inc"


#include "CRestrictCharacCreationEvent/StartEvent_1.inc"


#include "CRestrictCharacCreationEvent/EndEvent.inc"


#include "CRestrictCharacCreationEvent/getLimitCreateCharacter.inc"

