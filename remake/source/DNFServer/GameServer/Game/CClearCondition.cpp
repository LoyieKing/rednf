// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CClearCondition/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CClearCondition（G2 副本清除条件批次）
// ----------------------------------------------------------------------------
// ORIG：
//   C1    _ZN15CClearConditionC1Ev                    0x82a4342（vector 构造 + Clear）
//   Clear _ZN15CClearCondition5ClearEv                0x82a438a
//   Empty _ZN15CClearCondition5EmptyEv               0x822cba8 → return m_vec.empty()
//   IsC   _ZN15CClearCondition16IsClearConditionEv   0x822cb92 → return m_flag <= m_count
//   ClearCondition _ZN15CClearCondition14ClearConditionE... 0x82fefce
//     → type∈[0,4] 校验；遍历 (+0x14 指向的 vector<DungeonClearCondition>) 与
//       m_vec 同步递增；结尾 return (m_flag <= m_count)。
// 布局：当前 CClearCondition.h 以 m_unk14(int) 建模 +0x14（CBattle_Field 0x18
// 布局锁定）。ORIG 此处实为 vector<DungeonClearCondition>* 指针（指向外部表，
// SetDungeonClearConditionVector 写入 vec 地址；Clear 置 0），与 m_unk14 兼容。
// Empty/IsClearCondition/Clear 与布局一致，按 ORIG 实现。
// ============================================================================

#include "CClearCondition.h"

#include "CClearCondition/CClearCondition.inc"
}

#include "CClearCondition/dtor_CClearCondition.inc"
}

#include "CClearCondition/Clear.inc"
}

#include "CClearCondition/Empty.inc"
}

#include "CClearCondition/IsClearCondition.inc"
}

// [b28d-51] 判词 N（ORIG 0x082fefce，N82 族）：switch 决策树 + 帧槽方向差；
// 语义/调用集齐，byte 层封顶。
// [F2d-C05② 甄别] 无「两具名局部」比较（比较对象为 enum/int 形参 + 指针解引用）⇒ F2 不适用；
//   A/B 实测删 `(int)type` 零效（show_func_diff 两态同形 orig=110 our=112 gap=-2 ndiff=18；
//   ORIG-only={jg×2} vs OURS-only={mov,jle×2,jmp} = 块排布镜像 N82/N93附）⇒ 回滚维持原形。
#include "CClearCondition/ClearCondition.inc"
}

// [b28d-51] 判词 N（ORIG 0x082fef12，N82/N86 族）：vector 迭代器/临时槽
// 阶梯方向差（ORIG 帧 0x28 vs 4.4 0x24+ebx）；语义/调用集齐，byte 层封顶。
#include "CClearCondition/SetDungeonClearConditionVector.inc"
}
