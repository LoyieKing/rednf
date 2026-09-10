// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBurnigGoldMonsterEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 燃烧的金币怪物事件 CBurnigGoldMonsterEvent 实现。
// 报告缺失（§3.2 兜底）：ORIG 全部 W 弱符号，nm 定址 + objdump 逐指令转写：
//   C1 0x08116ee6 / D1 0x08116f02 / D0 0x08116f32 / StartEvent 0x08116f50 /
//   StartEvent(Word_Param) 0x08116fa2（thunk）/ EndEvent 0x08116fbc；
//   vtable 0x08b4f3e0；PRETTY_FUNCTION 与日志字符串 .rodata 实测。
#include "CBurnigGoldMonsterEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x08116ee6：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CBurnigGoldMonsterEvent/CBurnigGoldMonsterEvent.inc"
}

// ORIG 0x08116f02 / 0x08116f32：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CBurnigGoldMonsterEvent/dtor_CBurnigGoldMonsterEvent.inc"
}

// ORIG 0x08116f50：SetEventFlag(true) + cMyTrace(PRETTY, 0x12, 0)
//   ("CBurnigGoldMonsterEvent Event Start!")。
#include "CBurnigGoldMonsterEvent/StartEvent.inc"
}

// ORIG 0x08116fa2（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
// [b28c-30] N7 封顶：ORIG 间接 call 目标 %edx（`mov (%eax),%edx; …; call *%edx`）
// vs 4.4 恒 %eax，助记符全等仅寄存器名差（同 CPCRoomWorldDropEvent 件）。
#include "CBurnigGoldMonsterEvent/StartEvent_1.inc"
}

// ORIG 0x08116fbc：SetEventFlag(false) + cMyTrace(PRETTY, 0x1c, 0)
//   ("CBurnigGoldMonsterEvent Event End!")。
#include "CBurnigGoldMonsterEvent/EndEvent.inc"
}
