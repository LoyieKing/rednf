// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTournamentPvPEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 锦标赛 PvP 频道事件 CTournamentPvPEvent 实现。
// 报告：docs/class_func_reports/CTournamentPvPEvent/ 全部 6 个方法（已完整阅读
// CTournamentPvPEvent / ~CTournamentPvPEvent / ~CTournamentPvPEvent_08284618 /
// StartEvent / StartEvent_0828463c / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN19CTournamentPvPEventC2Ev`（ORIG 0x828471f）；ctor 已是首强符号。
#include <iostream>

#include "CTournamentPvPEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x082845cc：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CTournamentPvPEvent/CTournamentPvPEvent.inc"
}

// ORIG 0x082845e8 / 0x08284618：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CTournamentPvPEvent/dtor_CTournamentPvPEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08284636：空体（仅 push/mov/pop/ret，无 SetEventFlag、无 trace）。
#include "CTournamentPvPEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0828463c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)
// ("Tournament PvP Channel Open Start")。
#include "CTournamentPvPEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0828468e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)
// ("Tournament PvP Channel Open End")。
#include "CTournamentPvPEvent/EndEvent.inc"
}
