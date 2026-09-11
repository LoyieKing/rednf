// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMaxFatigueFactorEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 最大疲劳系数事件 CMaxFatigueFactorEvent 实现。
// 报告：docs/class_func_reports/CMaxFatigueFactorEvent/ 全部 6 个方法
// （CMaxFatigueFactorEvent / ~CMaxFatigueFactorEvent /
//  ~CMaxFatigueFactorEvent_081b276e / StartEvent / StartEvent_081b27e8 /
//  EndEvent）。
#include "CMaxFatigueFactorEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "CLocalChina_User.h"      // CLocalChina_User::SendFatigueEvent（static）
#include "GlobalData.h"
#include "GameWorld.h"
#include "CUser.h"                 // CUser / _Charac_info 完整定义（含 CUserCharacInfo）

extern int DEFAULT_MAX_FATIGUE;    // CUserCharacInfo.h 亦声明（ORIG B 0x9370f5c）

#include "CMaxFatigueFactorEvent/SendFatigueEvent.inc"


// [SplitC 2026-09-11 · TU 迁移] CMaxFatigueFactorEvent ctor 已迁出至 MaxFatigueFactorEvent.cpp（ORIG GI 键对齐）。



#include "CMaxFatigueFactorEvent/dtor_CMaxFatigueFactorEvent.inc"


#include "CMaxFatigueFactorEvent/StartEvent.inc"


#include "CMaxFatigueFactorEvent/StartEvent_1.inc"


#include "CMaxFatigueFactorEvent/EndEvent.inc"

