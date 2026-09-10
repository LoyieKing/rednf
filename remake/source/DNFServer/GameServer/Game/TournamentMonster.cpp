// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TournamentMonster/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r TournamentMonster（G2-4 竞技场怪物表项，8 字节：mobId + rate）
// ORIG：Clear 0830e4bc / Ctor 0830e4d4 / Dtor 089f1bc0（弱）。
// 布局与 CBattle_Field.h / CMap.h 的 struct TournamentMonster 一致。
// ============================================================================

struct TournamentMonster
{
    int m_mobId;  // +0x00
    int m_rate;   // +0x04

    TournamentMonster();
    ~TournamentMonster();
    void Clear();
};

#include "TournamentMonster/TournamentMonster.inc"
}

#include "TournamentMonster/Clear.inc"
}
