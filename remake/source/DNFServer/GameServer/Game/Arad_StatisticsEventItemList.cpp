// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_StatisticsEventItemList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD::Arad_StatisticsEventItemList 实现。
//
// 依据 docs/class_func_reports/ARAD__Arad_StatisticsEventItemList/*（ORIG elf32 反汇编
// + Ghidra C）。loadScriptData 为固定 30 个物品索引的 push_back 序列，逐项忠实转写。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN4ARAD28Arad_StatisticsEventItemListC2Ev`（ORIG 0x819d16f）；ctor 已是首强符号。
#include <iostream>

#include "Arad_StatisticsEventItemList.h"

#include <algorithm>
#include <utility>

namespace ARAD
{

#include "Arad_StatisticsEventItemList/Arad_StatisticsEventItemList.inc"
}

#include "Arad_StatisticsEventItemList/dtor_Arad_StatisticsEventItemList.inc"
}

#include "Arad_StatisticsEventItemList/loadScriptData.inc"
}

#include "Arad_StatisticsEventItemList/clearScriptData.inc"
}

#include "Arad_StatisticsEventItemList/needStatistics.inc"
}

}  // namespace ARAD
