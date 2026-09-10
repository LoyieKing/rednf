// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CVendingMachineBonusEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 自动售货机 Bonus 事件 CVendingMachineBonusEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x08116dbe / D1 0x08116dda / D0 0x08116e0a /
//   StartEvent() 0x08116e28 / StartEvent(Word_Param) 0x08116e7a /
//   EndEvent 0x08116e94（均弱符号 W，独立 TU 定义）。
#include "CVendingMachineBonusEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08116dbe：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CVendingMachineBonusEvent/CVendingMachineBonusEvent.inc"
}

// ORIG 0x08116dda / 0x08116e0a：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CVendingMachineBonusEvent/dtor_CVendingMachineBonusEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116e28：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x12,0)
// （ORIG 局部静态符号 0x08b4edc0 "virtual void CVendingMachineBonusEvent::StartEvent()"，
//  日志串 0x08b4dca8 "CVendingMachineBonusEvent Bonus Event Start!"）。
#include "CVendingMachineBonusEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116e7a：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（12=12）。
#include "CVendingMachineBonusEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116e94：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// （ORIG 局部静态符号 0x08b4ed80 "virtual void CVendingMachineBonusEvent::EndEvent()"，
//  日志串 0x08b4dccc "CVendingMachineBonusEvent Bonus Event End!"）。
#include "CVendingMachineBonusEvent/EndEvent.inc"
}
