// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPCRoomWorldDropEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r PC 房世界掉落事件 CPCRoomWorldDropEvent 实现。
// 报告：docs/class_func_reports/CPCRoomWorldDropEvent/（C1/D1/D0 有报告；
// StartEvent/StartEvent(Word_Param)/EndEvent 为 ORIG W 弱符号
// 0x082678be/0x08267910/0x0826792a，objdump 转写）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CPCRoomWorldDropEventC2Ev`（ORIG 0x82678a2）；ctor 已是首强符号。
#include <iostream>

#include "CPCRoomWorldDropEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x082677f8：仅基类构造 + vptr 置位。
#include "CPCRoomWorldDropEvent/CPCRoomWorldDropEvent.inc"
}

// ORIG 0x08267814 / 0x08267844：空体 + 虚析构由编译器生成同形。
#include "CPCRoomWorldDropEvent/dtor_CPCRoomWorldDropEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x082678be：置事件标志并打日志（__FUNCTION__ "StartEvent"，line 0x21）。
#include "CPCRoomWorldDropEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08267910：vtable+0x8 直调 StartEvent()（转发无参版本）。
// [b28c-30] N7 封顶：ORIG 取槽后 `mov (%eax),%edx; mov 0x8(%ebp),%eax;
// mov %eax,(%esp); call *%edx`（4.1 间接目标 %edx）；4.4 恒目标 %eax
// （`mov (%eax),%eax; …; call *%eax`）。助记符序列全等仅寄存器名差。
#include "CPCRoomWorldDropEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0826792a：清事件标志并打日志（__FUNCTION__ "EndEvent"，line 0x2c）。
#include "CPCRoomWorldDropEvent/EndEvent.inc"
}
