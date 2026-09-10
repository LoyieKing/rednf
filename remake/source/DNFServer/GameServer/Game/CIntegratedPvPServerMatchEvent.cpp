// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CIntegratedPvPServerMatchEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 整合 PvP 服务器匹配事件 CIntegratedPvPServerMatchEvent 实现。
// 报告：docs/class_func_reports/CIntegratedPvPServerMatchEvent/ 全部 6 个方法
// （CIntegratedPvPServerMatchEvent / ~CIntegratedPvPServerMatchEvent /
//  ~CIntegratedPvPServerMatchEvent_0811d484 / StartEvent /
//  StartEvent_0811d4f4 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN30CIntegratedPvPServerMatchEventC2Ev`（ORIG 0x811d5a1）；ctor 已是首强符号。
#include <iostream>

#include "CIntegratedPvPServerMatchEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811d438：仅基类构造 + vptr 置位（无自有成员）。
#include "CIntegratedPvPServerMatchEvent/CIntegratedPvPServerMatchEvent.inc"
}

// ORIG 0x0811d454 / 0x0811d484：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CIntegratedPvPServerMatchEvent/dtor_CIntegratedPvPServerMatchEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811d4a2：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x12,0)("CIntegratedPvPServerMatchEvent Start!")。
#include "CIntegratedPvPServerMatchEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811d4f4：与 StartEvent() 同体（file 串 "StartEvent"，行 0x18；
// 忽略 param）。
#include "CIntegratedPvPServerMatchEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811d546：仅 SetEventFlag(false)（无 trace）。
#include "CIntegratedPvPServerMatchEvent/EndEvent.inc"
}
