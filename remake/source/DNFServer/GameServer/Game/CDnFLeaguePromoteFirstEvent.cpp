// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDnFLeaguePromoteFirstEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r DnF 联赛晋级第一事件 CDnFLeaguePromoteFirstEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CDnFLeaguePromoteFirstEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c0ba（弱 W）：仅基类构造 + vptr 置位。
#include "CDnFLeaguePromoteFirstEvent/CDnFLeaguePromoteFirstEvent.inc"
}

// ORIG 0x0811c0d6 / 0x0811c106（弱 W）：标准虚析构（空体编译器生成同形）。
#include "CDnFLeaguePromoteFirstEvent/dtor_CDnFLeaguePromoteFirstEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c124：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x13,0)
// ("CDnFLeaguePromoteFirstEvent Event Start!")。
#include "CDnFLeaguePromoteFirstEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c176：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（12=12）。
#include "CDnFLeaguePromoteFirstEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c190：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1d,0)
// ("CDnFLeaguePromoteFirstEvent Event End!")。
#include "CDnFLeaguePromoteFirstEvent/EndEvent.inc"
}
