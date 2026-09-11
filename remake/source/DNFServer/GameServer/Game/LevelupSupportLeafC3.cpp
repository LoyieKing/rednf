// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LevelupSupportLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "GameTypes.h"                 // cMyTrace
#include "LevelupSupportEventManger.h" // 四类权威声明

#include "LevelupSupportLeafC3/loadRDARScriptFile.inc"
#include "LevelupSupportLeafC3/unloadRDARScriptFile.inc"
#include "LevelupSupportLeafC3/ScanType.inc"
#include "LevelupSupportLeafC3/ScanInt.inc"
#include "LevelupSupportLeafC3/ScanTypeOrInt.inc"
#include "LevelupSupportLeafC3/ScanStr.inc"

#include "LevelupSupportLeafC3/getCharacterJobToEnumValue.inc"

// ---- 布局断言（头文件注释 + 各 ctor 报告反汇编实证）----
typedef char lvl_manger_size_assert[sizeof(LevelupSupportEventManger) == 0x40 ? 1 : -1];
typedef char lvl_2nd_manger_size_assert[sizeof(LevelupSupport2ndEventManger) == 0x28 ? 1 : -1];
typedef char lvl_sciprt_size_assert[sizeof(LevelupSupportSciprt) == 0x18 ? 1 : -1];
typedef char lvl_2nd_sciprt_size_assert[sizeof(LevelupSupport2ndScript) == 0x24 ? 1 : -1];

#include "LevelupSupportLeafC3/Load.inc"


#include "LevelupSupportLeafC3/Destroy.inc"

// ============================================================================
// LevelupSupportEventManger::GetDefaultEventInfo —— 报告 GetDefaultEventInfo.md
// @0x08147012（_ZNK25LevelupSupportEventManger19GetDefaultEventInfoEv，sz 0xb）：
// 仅返回 &m_events（this+0x34）的 const 引用。ORIG 唯一调用点
// Inter_LoadEventLevelupSupport::dispatch_sig @0x84e8d5f 把返回值直接作为
// vector<EventInfo>::operator=(const vector&) 的源实参（0x84f16ae），实证
// 引用返回 ABI；故头声明为 const std::vector<EventInfo>&（本次修正）。
// ============================================================================
const std::vector<LevelupSupportEventManger::EventInfo>&
#include "LevelupSupportLeafC3/GetDefaultEventInfo.inc"


#include "LevelupSupportLeafC3/Load_1.inc"


#include "LevelupSupportLeafC3/Destroy_1.inc"


#include "LevelupSupportLeafC3/loadScript.inc"


#include "LevelupSupportLeafC3/clearScript.inc"


#include "LevelupSupportLeafC3/loadScript_1.inc"


#include "LevelupSupportLeafC3/clearScript_1.inc"

