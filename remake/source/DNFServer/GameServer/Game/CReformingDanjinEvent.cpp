// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CReformingDanjinEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 改造丹金事件 CReformingDanjinEvent 实现。
// 报告：docs/class_func_reports/CReformingDanjinEvent/ 全部 6 个方法（已完整
// 阅读 CReformingDanjinEvent / ~CReformingDanjinEvent /
//  ~CReformingDanjinEvent_0826e978 / StartEvent / StartEvent_0826e9e8 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CReformingDanjinEventC2Ev`（ORIG 0x826eacb）；ctor 已是首强符号。
#include <iostream>

#include "CReformingDanjinEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826e92c：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CReformingDanjinEvent/CReformingDanjinEvent.inc"
}

// ORIG 0x0826e948 / 0x0826e978：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CReformingDanjinEvent/dtor_CReformingDanjinEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0826e996：SetEventFlag(true) +
// cMyTrace("virtual void CReformingDanjinEvent::StartEvent()",0x13,0)
// ("Reforming Danjin Event Start!")。
#include "CReformingDanjinEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0826e9e8：与 StartEvent() 同体（SetEventFlag(true) + trace，
// file 串为 "virtual void CReformingDanjinEvent::StartEvent(Word_Param)"，
// 行 0x19）。
#include "CReformingDanjinEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0826ea3a：SetEventFlag(false) +
// cMyTrace("virtual void CReformingDanjinEvent::EndEvent()",0x1f,0)
// ("Reforming Danjin Event End!")。
#include "CReformingDanjinEvent/EndEvent.inc"
}
