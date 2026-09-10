// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPCRoomFatigueEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CPCRoomFatigueEvent 实现（网吧疲劳活动事件）。
// 依据 docs/class_func_reports/CPCRoomFatigueEvent/*.md + ORIG elf 反汇编：
//   C1 0x082675c0：CEventBase() + vptr
//   D1 0x082675dc / D0 0x0826760c：vptr 恢复 + ~CEventBase
//   StartEvent 0x0826762a / StartEvent(Word_Param) 0x0826767c：SetEventFlag + cMyTrace
//   EndEvent 0x082676ce：SetEventFlag(false) + cMyTrace
//   IsEventing 0x08267720：基类判总开关(NULL 参数) + user 非空 + isAffectedPremium(0xc)
#include "CPCRoomFatigueEvent.h"

#include "CUser.h"          // isAffectedPremium

#include "CPCRoomFatigueEvent/CPCRoomFatigueEvent.inc"
}

#include "CPCRoomFatigueEvent/dtor_CPCRoomFatigueEvent.inc"
}

#include "CPCRoomFatigueEvent/StartEvent.inc"
}

#include "CPCRoomFatigueEvent/StartEvent_1.inc"
}

#include "CPCRoomFatigueEvent/EndEvent.inc"
}

#include "CPCRoomFatigueEvent/IsEventing.inc"
}
