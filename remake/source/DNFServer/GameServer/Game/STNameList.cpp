// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STNameList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— STNameList（字符串名 -> ID 映射表，见 STNameList.md）
//   STNameList ctor 0x088baef2（调 m_map ctor + Clear）
//   Clear         0x088baf34（m_map.clear()）
//   GetID         0x088baf48（m_map.get(name)，未命中 0xffffffff）
// ============================================================================

#include <cstring>
#include "STNameList.h"

#include "STNameList/STNameList.inc"
}

#include "STNameList/Clear.inc"
}

#include "STNameList/GetID.inc"
}
