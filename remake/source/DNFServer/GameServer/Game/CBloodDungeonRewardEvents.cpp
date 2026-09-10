// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBloodDungeonRewardEvents/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CBloodDungeonRewardEvents.cpp —— 血之副本结算金币 repeat 事件实现
// （2026-08-24，EventRewardGoldC2 批次；独占 TU）
//
// 覆盖 ORIG 全部真实符号（nm 实证均为 W = 头内联强度，本 TU 以类外定义提供同名符号）：
//   CBloodDungeonRewardFirstEvent（event id 0x2d）：
//     C1/C2 0x0811bdde / D1/D2 0x0811be04 / D0 0x0811be34 / StartEvent() 0x0811bd12 /
//     StartEvent(Word_Param) 0x0811bd64 / EndEvent() 0x0811bd8c / GetGold W 0x085bff1a
//   CBloodDungeonRewardSecondEvent（event id 0x2e）：
//     C1/C2 0x0811bf1e / D1/D2 0x0811bf44 / D0 0x0811bf74 / StartEvent() 0x0811be52 /
//     StartEvent(Word_Param) 0x0811bea4 / EndEvent() 0x0811becc / GetGold W 0x085bff26
//
// 日志常量为 ORIG .rodata 实测：__PRETTY_FUNCTION__ 串与 "…Start!/End!" 格式串、
// cMyTrace 行号（Start=0x10；First End=0x2d、Second End=0x1b）。
// 唯一业务调用方 CParty::make_blood_dungeon_info（ORIG T 0x0859e0d2）。
// ============================================================================

#include "CBloodDungeonRewardFirstEvent.h"
#include "CBloodDungeonRewardSecondEvent.h"

// ---------------------------------------------------------------------------
// CBloodDungeonRewardFirstEvent
// ---------------------------------------------------------------------------

#include "CBloodDungeonRewardEvents/CBloodDungeonRewardFirstEvent.inc"
}

#include "CBloodDungeonRewardEvents/dtor_CBloodDungeonRewardFirstEvent.inc"
}

#include "CBloodDungeonRewardEvents/StartEvent.inc"
}

#include "CBloodDungeonRewardEvents/StartEvent_1.inc"
}

#include "CBloodDungeonRewardEvents/EndEvent.inc"
}

#include "CBloodDungeonRewardEvents/GetGold.inc"
}

// ---------------------------------------------------------------------------
// CBloodDungeonRewardSecondEvent
// ---------------------------------------------------------------------------

#include "CBloodDungeonRewardEvents/CBloodDungeonRewardSecondEvent.inc"
}

#include "CBloodDungeonRewardEvents/dtor_CBloodDungeonRewardSecondEvent.inc"
}

#include "CBloodDungeonRewardEvents/StartEvent_2.inc"
}

#include "CBloodDungeonRewardEvents/StartEvent_3.inc"
}

#include "CBloodDungeonRewardEvents/EndEvent_1.inc"
}

#include "CBloodDungeonRewardEvents/GetGold_1.inc"
}
