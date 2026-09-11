// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventBase/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CEventBase 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CGuildServerProxy.h 为准。
// 依据 docs/class_func_reports/CEventBase.md + ORIG elf 反汇编：
//   ctor 0x0810ae2c（调用 stADDINFO ctor 0x0810af10，+0x08 flag 置 0）
//   ~ctor 0x0810ae52 / StartEvent(Word_Param) 0x0810af26（空）
//   StartAction 0x080c84c4（空）/ AppendInfo 0x080c84ca（空）
//   GetAddInfo 0x080c84d0（返回 +0x04 的 4 字节）
//   SetEventFlag 0x080c84e2（写 +0x08）/ IsEventing 0x080c84fa（读 +0x08）
//   changeCharacName/dailyresetData/deliveryStart/deliveryEnd/delivery 空。
#include "CGuildServerProxy.h"
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10CEventBaseC2Ev 的前提，P338）

#include "CEventBase/CEventBase.inc"


#include "CEventBase/stADDINFO.inc"


#include "CEventBase/dtor_CEventBase.inc"


#include "CEventBase/StartEvent.inc"
#include "CEventBase/StartAction.inc"
#include "CEventBase/AppendInfo.inc"

#include "CEventBase/GetAddInfo.inc"


#include "CEventBase/changeCharacName.inc"
#include "CEventBase/dailyresetData.inc"
#include "CEventBase/deliveryStart.inc"
#include "CEventBase/deliveryEnd.inc"
#include "CEventBase/delivery.inc"

#include "CEventBase/IsEventing.inc"


#include "CEventBase/SetEventFlag.inc"

