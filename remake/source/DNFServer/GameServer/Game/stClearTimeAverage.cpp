// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 stClearTimeAverage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// stClearTimeAverage / stClearTimeAverageMapKey（排行系统清时平均）
// ----------------------------------------------------------------------------
// ORIG（df_game_r）：
//   stClearTimeAverage
//     ctor  _ZN18stClearTimeAverageC1Ev / C2Ev  0x0891b6a8（sub $0x18 + call clear）
//     clear _ZN18stClearTimeAverage5clearEv     0x0891b6bc（+0/+4/+8/+0xc/+0x10 五 int 置 0）
//   stClearTimeAverageMapKey
//     ctor  _ZN24stClearTimeAverageMapKeyC1Ev / C2Ev  0x08370d1c（call clear）
//     clear _ZN24stClearTimeAverageMapKey5clearEv     0x08370d30（+0/+4 两 int 置 0）
//     operator< _ZNK24stClearTimeAverageMapKeyltERKS_ 0x08370d48（W 头内联）
// 布局：stClearTimeAverage { int m_v[5]; }（0x14）；
//       stClearTimeAverageMapKey { int m_a; int m_b; }（8）。
// ============================================================================

#include "stClearTimeAverage.h"

#include "stClearTimeAverage/stClearTimeAverage.inc"
}

#include "stClearTimeAverage/clear.inc"
}

#include "stClearTimeAverage/stClearTimeAverageMapKey.inc"
}

#include "stClearTimeAverage/clear_1.inc"
}

bool stClearTimeAverageMapKey::operator<(const stClearTimeAverageMapKey& o) const
{
    if (m_a < o.m_a)
        return true;
    if (m_a == o.m_a && m_b < o.m_b)
        return true;
    return false;
}
