// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CConditionEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 条件事件 CConditionEvent 实现。
// 报告：docs/class_func_reports/CConditionEvent/（6 函数全读）。
#include "CConditionEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "LogManager.h"            // LogManager::logFormat

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit，使本 TU 发射
// `_GLOBAL__I__ZN15CConditionEvent10StartEventEv`（ORIG 0x8184a1a）；StartEvent() 定义
// 随之前移为本 TU 首个强外部符号（= GI 键名来源）。零语义/布局改动。
#include <iostream>

// ---------------------------------------------------------------------------
// ORIG 0x081848bc：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0xd,0)
// ("[ConditionEvent] Start!")。
#include "CConditionEvent/StartEvent.inc"
}

// ORIG 0x08184970：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CConditionEvent/CConditionEvent.inc"
}

// ORIG 0x0818498c / 0x081849bc：D1 重置 vptr 后调 CEventBase::~CEventBase（末尾
// __in_chrg 常量折叠不 delete）；D0 调 D1 后 operator delete。空体 + 虚析构由
// 编译器生成同形。
#include "CConditionEvent/dtor_CConditionEvent.inc"
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ORIG 0x0818490e：thunk——经 vtable slot 2（StartEvent()）虚调用。
#include "CConditionEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08184928：logFormat(1,"localjapan/Arad_ConditionEvent.cpp",
// __PRETTY_FUNCTION__,0x17,"[CConditionEvent] End Event.") 后 SetEventFlag(false)。
#include "CConditionEvent/EndEvent.inc"
}
