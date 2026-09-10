// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUXGameLogEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r U/X Game Log 事件 CUXGameLogEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x08117678 / D1 0x08117694 / D0 0x081176c4 /
//   StartEvent() 0x081176e2 / StartEvent(Word_Param) 0x08117734 /
//   EndEvent 0x0811774e（均弱符号 W，独立 TU 定义）。
#include "CUXGameLogEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08117678：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CUXGameLogEvent/CUXGameLogEvent.inc"
}

// ORIG 0x08117694 / 0x081176c4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CUXGameLogEvent/dtor_CUXGameLogEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x081176e2：SetEventFlag(true) + cMyTrace("CUXGameLogEvent::StartEvent()",0x13,0)
// ("CUXGameLogEvent Event Start!")。
#include "CUXGameLogEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08117734：thunk——经 vtable slot 2（StartEvent()）虚调用。
#include "CUXGameLogEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811774e：SetEventFlag(false) + cMyTrace("CUXGameLogEvent::EndEvent()",0x1e,0)
// ("CUXGameLogEvent Event End!")。
#include "CUXGameLogEvent/EndEvent.inc"
}
