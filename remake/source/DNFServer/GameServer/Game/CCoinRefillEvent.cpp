// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCoinRefillEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CCoinRefillEvent —— 金币补充 repeat 事件实现。
// 全部函数逐段对齐 docs/class_func_reports/CCoinRefillEvent/ 下报告：
//   CCoinRefillEvent.md(0x0832bdf4) / ~CCoinRefillEvent.md(0x0832be2e) /
//   StartEvent.md(0x0832be7c) / StartEvent_0832be82.md(0x0832be82) /
//   EndEvent.md(0x0832c048) / getLastEventTime.md(0x0832c0b8) /
//   getNextEventTime.md(0x0832c148) / registNextRefillTimer.md(0x0832c1fe)。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN16CCoinRefillEventC2Ev`（ORIG 0x832c399）；ctor 已是首强符号。
#include <iostream>

#include "CCoinRefillEvent.h"

#include <cstdio>
#include <cstring>
#include <ctime>

#include "CEnvironment.h"   // G_CEnvironment() / m_serverEnvir.m_fatigueTime（env+0x37c）
#include "CSystemTime.h"
#include "GlobalData.h"     // s_systemTime_ / s_event_manager
#include "TimerQueue.h"     // G_TimerQueue / InsertTimer

#include "CCoinRefillEvent/CCoinRefillEvent.inc"
}

#include "CCoinRefillEvent/dtor_CCoinRefillEvent.inc"
}

#include "CCoinRefillEvent/StartEvent.inc"
}

#include "CCoinRefillEvent/StartEvent_1.inc"
}

#include "CCoinRefillEvent/EndEvent.inc"
}

#include "CCoinRefillEvent/getLastEventTime.inc"
}

#include "CCoinRefillEvent/getNextEventTime.inc"
}

#include "CCoinRefillEvent/registNextRefillTimer.inc"
}
