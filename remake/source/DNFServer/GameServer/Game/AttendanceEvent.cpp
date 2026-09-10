// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AttendanceEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 签到事件 AttendanceEvent 实现。
// 报告：docs/class_func_reports/AttendanceEvent/ 全部 7 个方法已读。
//   C1 0x080c828c / D1 0x080c82a8 / D0 0x080c82d8 / StartEvent 0x080c82f6 /
//   StartEvent(Word_Param) 0x080c8312 / EndEvent 0x080c83f4 /
//   getRewordItem 0x080c8446。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15AttendanceEventC2Ev`（ORIG 0x80c84a8）；ctor 已是首强符号。
#include <iostream>

#include "AttendanceEvent.h"

#include "GameTypes.h"     // cMyTrace
#include "LogManager.h"    // LogManager::logFormat

// ORIG 0x080c828c：仅基类构造 + vptr 置位。
#include "AttendanceEvent/AttendanceEvent.inc"
}

// ORIG 0x080c82a8 / 0x080c82d8：空体 + 虚析构由编译器生成同形。
#include "AttendanceEvent/dtor_AttendanceEvent.inc"
}

// ORIG 0x080c82f6：仅 SetEventFlag(true)，无 trace。
#include "AttendanceEvent/StartEvent.inc"
}

// ORIG 0x080c8312：param 低 ushort → m_addInfo.m_info0(+0x04)、高 ushort →
// m_addInfo.m_info1(+0x06)；(short)m_info0 < 1 → logFormat 错误日志；否则
// 虚调 StartEvent() 后 cMyTrace(PRETTY, 0x1d, 0)("Attendance(AddInfo:%d,%d)
// Event Start!", (short)m_info0, (short)m_info1)。
#include "AttendanceEvent/StartEvent_1.inc"
}

// ORIG 0x080c83f4：SetEventFlag(false) + cMyTrace(PRETTY, 0x23, 0)
//   ("Attendacne Event End!")（ORIG 原文拼写 Attendacne）。
#include "AttendanceEvent/EndEvent.inc"
}

// ORIG 0x080c8446：直接 call CEventBase::GetAddInfo()（非虚，sret 写 out
// 指针；GetAddInfo 返回 stADDINFO，2026-08-28 OrigEventLeaf8f 修正 ABI），
// 返回 m_info0 符号扩展（movzwl+cwtl，与 ORIG 逐条同形；AE 实证）。
#include "AttendanceEvent/getRewordItem.inc"
}
