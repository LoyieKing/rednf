// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPvPExpPenaltyEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r PvP 经验惩罚事件 CPvPExpPenaltyEvent 实现。
// 报告：docs/class_func_reports/CPvPExpPenaltyEvent/ 全部 6 个方法（已完整
// 阅读 CPvPExpPenaltyEvent / ~CPvPExpPenaltyEvent /
//  ~CPvPExpPenaltyEvent_08268d68 / StartEvent / StartEvent_08268dd8 / EndEvent）。
#include "CPvPExpPenaltyEvent.h"

#include <iostream>          // std::ios_base::Init 静态初始化（ORIG _GLOBAL__I__ZN19CPvPExpPenaltyEventC2Ev 0x08268e85）
#include "GameTypes.h"       // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08268d1c：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CPvPExpPenaltyEvent/CPvPExpPenaltyEvent.inc"
}

// ORIG 0x08268d38 / 0x08268d68：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CPvPExpPenaltyEvent/dtor_CPvPExpPenaltyEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268d86：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x12,0)("CPvPExpPenaltyEvent Start!")。
// __FUNCTION__ 生成局部静态 _ZZN19CPvPExpPenaltyEvent10StartEventEvE12__FUNCTION__
// （.rodata 0x08bf3f1b 内容 "StartEvent"），勿用手写字面量。
#include "CPvPExpPenaltyEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268dd8：与 StartEvent() 同体（SetEventFlag(true) + trace，
// __FUNCTION__ 静态 0x08bf3f10 __FUNCTION__，行 0x18）。
#include "CPvPExpPenaltyEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268e2a：仅 SetEventFlag(false)，无 trace（sub $0x18 帧实证）。
#include "CPvPExpPenaltyEvent/EndEvent.inc"
}
