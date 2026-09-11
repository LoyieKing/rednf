// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TowerOfDespairMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// TowerOfDespairMgr.cpp —— TOD 域首批叶子实现（PartyDepsB2 批次）。
// 逐指令依据：
//   TOD_Layer::TOD_Layer            docs/class_func_reports/TOD_Layer/TOD_Layer.md
//   TowerOfDespairMgr::SendAPCInfo  docs/class_func_reports/TowerOfDespairMgr/SendAPCInfo.md
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN9TOD_LayerC2Et`（ORIG 0x85ff0dd）；TOD_Layer ctor 已是首强符号。
#include <iostream>

#include "TowerOfDespairMgr.h"
#include <string.h>

#include "GameTypes.h"   // cMyTrace(char const*, int, int) / operator()(char const*, ...)

#include "TowerOfDespairMgr/TOD_Layer.inc"


#include "TowerOfDespairMgr/SendAPCInfo.inc"


#include "TowerOfDespairMgr/IsTowerOfDespairDungeon_ByIndex.inc"


#include "TowerOfDespairMgr/set_data.inc"


#include "TowerOfDespairMgr/Set_APC_Info.inc"


#include "TowerOfDespairMgr/Reset_APC_Info.inc"


#include "TowerOfDespairMgr/ResetAPCInfo.inc"


#include "TowerOfDespairMgr/SetAPCInfo.inc"


#include "TowerOfDespairMgr/Reload_APC_Info.inc"


#include "TowerOfDespairMgr/_layerInfo.inc"

