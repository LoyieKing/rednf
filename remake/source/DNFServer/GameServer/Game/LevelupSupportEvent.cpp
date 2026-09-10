// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LevelupSupportEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 升级支持事件 LevelupSupportEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x081169f8 / D1 0x08116a14 / D0 0x08116a44 /
//   StartEvent() 0x08116a62 / StartEvent(Word_Param) 0x08116ab4 /
//   EndEvent 0x08116ace（均弱符号 W，独立 TU 定义）。
#include "LevelupSupportEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081169f8：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "LevelupSupportEvent/LevelupSupportEvent.inc"
}

// ORIG 0x08116a14 / 0x08116a44：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "LevelupSupportEvent/dtor_LevelupSupportEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116a62：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x3f,0)
// ("LevelupSupportEvent Event Start!")。
#include "LevelupSupportEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116ab4：thunk——经 vtable slot 2（StartEvent()）虚调用。
#include "LevelupSupportEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116ace：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x49,0)
// ("LevelupSupportEvent Event End!")。
#include "LevelupSupportEvent/EndEvent.inc"
}
