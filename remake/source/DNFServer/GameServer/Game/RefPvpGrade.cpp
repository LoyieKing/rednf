// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RefPvpGrade/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// RefPvpGrade 权威实现。布局与语义按 ORIG 反汇编逐条转写（见 RefPvpGrade.h 注释）。
// ORIG 符号（T）：
//   _ZN11RefPvpGradeC1Ev (0x085d46e6) / D1Ev (0x085d470a)
//   _ZN11RefPvpGrade5ClearEv (0x085d4722)
//   _ZN11RefPvpGrade12SetLastGradeEi (0x085d4768)
//   _ZN11RefPvpGrade12GetLastGradeEv / _ZN11RefPvpGrade11SetPvpGradeEiRKNS_15RefPvpGradeDataE
//   _ZNK11RefPvpGrade11GetPvpGradeEi / GetPvpPointEi / GetPvpNextRankPointEi / GetPvpCurrRankPointEi
//   _ZN11RefPvpGrade3setERK15stPvpRefTable_t (0x085d48a0)

#include "RefPvpGrade.h"

// ORIG ctor 0x85d46e6：构造 vector@+0x140（成员自动），随后 Clear()。
#include "RefPvpGrade/RefPvpGrade.inc"
}

// ORIG dtor 0x85d470a：仅 vector@+0x140 析构（成员自动）。
#include "RefPvpGrade/dtor_RefPvpGrade.inc"
}
// ORIG W 0x0844d54c：返回 this+0x134（m_pvpExpLoss1）。
#include "RefPvpGrade/getPvPExpLossGrade.inc"
}

// ORIG W 0x0844d55a：返回 this+0x138（m_pvpExpLoss2）。
#include "RefPvpGrade/getPvPExpLossExp.inc"
}

// ORIG W 0x0844d568：返回 this+0x13c（m_rankRateGrade）。
#include "RefPvpGrade/getPvPRankRateGrade.inc"
}

// ORIG W 0x0844d576：mov 0x8(%ebp),%eax; add $0x140,%eax; ret
// —— 返回 this+0x140（m_rankRate 对象地址），即 vector 引用。
#include "RefPvpGrade/getPvPRankRate.inc"
}

// ORIG Clear 0x85d4722：this+0 = 0；循环 i=0..0x25：this+4+i*8=0、this+8+i*8=0。
#include "RefPvpGrade/Clear.inc"
}

// ORIG SetLastGrade 0x85d4768：grade<0 || grade>0x26 → 0；否则 this+0=grade，返回 1。
#include "RefPvpGrade/SetLastGrade.inc"
}

// ORIG GetLastGrade 0x85d478e：返回 this+0。
#include "RefPvpGrade/GetLastGrade.inc"
}

// ORIG SetPvpGrade 0x85d4798：grade<0 || grade>0x25 → 0；否则写 data[grade]，返回 1。
#include "RefPvpGrade/SetPvpGrade.inc"
}

// ORIG GetPvpGrade 0x85d47ce：idx < this+0xc(=m_data[1].curr) → 0；
// 循环 i=1..<GetLastGrade()：m_data[i].curr < idx && m_data[i].next >= idx → i；否则 -1。
#include "RefPvpGrade/GetPvpGrade.inc"
}

// ORIG GetPvpPoint 0x85d4832：idx < 1 → 0；idx >= 0x26 → +0x12c(=m_data[0x25].curr)；
// 否则 this+idx*8+4(=m_data[idx].curr)。
#include "RefPvpGrade/GetPvpPoint.inc"
}

// ORIG GetPvpNextRankPoint 0x85d4860：idx >= 0x26 → +0x130(=m_data[0x25].next)；否则 m_data[idx].next。
#include "RefPvpGrade/GetPvpNextRankPoint.inc"
}

// ORIG GetPvpCurrRankPoint 0x85d4880：idx >= 0x26 → +0x12c(=m_data[0x25].curr)；否则 m_data[idx].curr。
#include "RefPvpGrade/GetPvpCurrRankPoint.inc"
}

// ORIG set 0x85d48a0：
//   this+0x134 = param+0x10 (m_pvpExpLoss1)
//   this+0x138 = param+0x14 (m_pvpExpLoss2)
//   this+0x13c = param+0x18 (m_rankRateGrade)
//   vector@+0x140 operator= param+0x1c (m_rankRate)
#include "RefPvpGrade/set.inc"
}
