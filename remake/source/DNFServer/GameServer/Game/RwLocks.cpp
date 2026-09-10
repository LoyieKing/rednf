// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RwLocks/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ RwSpinLock + SectionLock + RwSectionLock 实现（独立 TU，仅 ORIG 符号）。
// 证据：docs/class_func_reports/RwSpinLock/（11 函数报告）、
// docs/class_func_reports/SectionLock/（7 函数报告）；
// RwSectionLock 无报告目录 → nm 定址后 objdump -d 0x08ad37f8..0x08ad38a0 转写（§3.2 标注报告缺失）。
// RwSpinLock state 位布局：0x80000000=写持有，0x40000000=写等待，低 30 位=读计数；
// enter/leave 原子 CAS 自旋，失败 YieldProcessor 重试（ORIG 0x8ad363a/0x8ad36be 实证）。
#include "RwSpinLock.h"
#include "RwSectionLock.h"
#include "SectionLock.h"

// ---- YieldProcessor（ORIG 0x8ad351c，空函数；static 每 TU 一份，与 SpinLock.cpp 同做法）----
static void YieldProcessor() {}

// ---- RwSpinLock（ORIG T 0x08ad35a6..0x08ad3741）----
#include "RwLocks/RwSpinLock.inc"

#include "RwLocks/enterRead.inc"

#include "RwLocks/leaveRead.inc"

#include "RwLocks/enterWrite.inc"

#include "RwLocks/leaveWrite.inc"

#include "RwLocks/isReadTaken.inc"

#include "RwLocks/isWriteTaken.inc"

#include "RwLocks/enterReadToValue.inc"
}

#include "RwLocks/leaveReadToValue.inc"
}

#include "RwLocks/enterWriteToValue.inc"
}

#include "RwLocks/leaveWriteToValue.inc"
}

// ---- SectionLock（ORIG T 0x08ad3742..0x08ad37f7）----
#include "RwLocks/SectionLock.inc"
}

#include "RwLocks/dtor_SectionLock.inc"

#include "RwLocks/enter.inc"

#include "RwLocks/tryEnter.inc"

#include "RwLocks/leave.inc"

#include "RwLocks/isTaken.inc"

#include "RwLocks/isTakenByCurrentThread.inc"
#include "RwLocks/RwSectionLock.inc"
}

#include "RwLocks/dtor_RwSectionLock.inc"

#include "RwLocks/enterRead_1.inc"

#include "RwLocks/leaveRead_1.inc"

#include "RwLocks/enterWrite_1.inc"

#include "RwLocks/leaveWrite_1.inc"
