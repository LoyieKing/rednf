// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventAdvanceAltarOpen/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 进阶祭坛开启事件 CEventAdvanceAltarOpen 实现。
// 报告：docs/class_func_reports/CEventAdvanceAltarOpen/（3 函数全读）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22CEventAdvanceAltarOpen10StartEventEv`（ORIG 0x80e95c5）；
// 本 TU 的 C1/D1 均为 W 弱符号（ORIG 同）⇒ 首强符号 = StartEvent。
#include <iostream>

#include "CEventAdvanceAltarOpen.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x080e94dc 空体（仅 push/pop/ret）。
// ORIG 的 C1/D1 为头内 inline 弱符号（不参与键名）⇒ ORIG 键名 = StartEvent；
// 我方 ctor 为外联强符号，故必须把 StartEvent 定义置于其前。
#include "CEventAdvanceAltarOpen/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116b2c（弱 W）：调 CEventBase::CEventBase(0x0810ae2c) 后
// `movl $0x8b314e8,(%eax)` 置 vptr（vtable+8），无自有成员初始化。
#include "CEventAdvanceAltarOpen/CEventAdvanceAltarOpen.inc"
}

// ORIG 0x080e95e2（弱 W）：D1 `movl $0x8b314e8,(%eax)` 重置 vptr（vtable+8）后
// call CEventBase::~CEventBase(0x0810ae52)，`mov $0x0,%eax; test %al,%al` 判定
// deleting 标志为假；D0 0x080e9612 调 D1 后 operator delete。空体 + 虚析构
// 由编译器生成同形。
#include "CEventAdvanceAltarOpen/dtor_CEventAdvanceAltarOpen.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x080e94e2：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x48e,0)
// ("CEventAdvanceAltarOpen Event Start! "，尾部带空格)。
#include "CEventAdvanceAltarOpen/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x080e9534：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x494,0)
// ("CEventAdvanceAltarOpen Event End!")。
#include "CEventAdvanceAltarOpen/EndEvent.inc"
}
