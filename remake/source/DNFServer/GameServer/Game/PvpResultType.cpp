// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PvpResultType/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r PvpResultType（G2-4 PvP 结果类型，0x84 字节）
// 逐函数对照 docs/class_func_reports/PvpResultType.md 与 ORIG 反汇编实现；
// AddNewResult 使用 AverageIncremental<int,long long>（ORIG 085dfec6）。
// 2026-08-28 HdrFieldG：删除本 TU 局部类定义，改用权威 PvpResultType.h
// （原局部 m_fieldN 名 → 权威字段名映射：m_field0→m_totalWinCount、
//  m_field4→m_totalLoseCount、m_field8→m_expPoint、m_fieldc→m_oldExpPoint、
//  m_field14→m_pvpGrade、m_field5c→m_todayWinCount、m_field60→m_todayLoseCount、
//  m_field64→m_todayPlayCount、m_field68→m_nextResetTime；其余同名）。
#include "PvpResultType.h"

// ---- AverageIncremental（mangling _Z18AverageIncrementalIixET_T0_S1_S1_）----
template <typename T, typename T2>
T AverageIncremental(T2 a, T2 b, T2 count)
{
    return (T)((a * count + b) / (count + 1));
}

#include "PvpResultType/PvpResultType.inc"
}

#include "PvpResultType/dtor_PvpResultType.inc"
}

#include "PvpResultType/Set.inc"
}

#include "PvpResultType/Get.inc"
}

#include "PvpResultType/AddNewResult.inc"
}

#include "PvpResultType/Clear.inc"
}

// [L10-2 P210②] GetTotalScore 的 out-of-line 定义迁往 PvP_Room.cpp（置于
// SetPVPResult 之前）：4.4 只对「本 TU 内定义且先于 caller」的 callee 精确继承
// outgoing 帧（ORIG 0x085dc034 `sub $0x4`；跨 TU 仅声明时保守 `sub $0x14`）。
// 全仓唯一消费者 = PvP_Room::SetPVPResult（grep 实证），迁移零副作用；
// ORIG 中 GetTotalScore @0x085d5efe 与 PvP_Room 段（0x085d6012..）地址相邻，
// 本身即同 TU 旁证。
// ============================================================================
// PvpResultSet（G2-4 PvP 结果集合，PvpResultType 派生薄封装）
// 三个薄封装定义原在 PvpResultSet.cpp——ORIG outgoing sub $0x4 为 4.1 对同 TU
// callee（PvpResultType C1/D1/Clear）的精确分配；跨 TU 时 4.4 保守分配 0x18。
// 迁入本 TU（include PvpResultSet.h）后 unit-at-a-time 已知 callee，恢复 0x4。
// ============================================================================
#include "PvpResultSet.h"

#include "PvpResultType/PvpResultSet.inc"
}

#include "PvpResultType/dtor_PvpResultSet.inc"
}

#include "PvpResultType/Clear_1.inc"
}
