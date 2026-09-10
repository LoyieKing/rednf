// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CArtifact/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— user_creature::CArtifact（4B 单指针类，权威 TU）
// ORIG 符号（nm -S --defined-only 实证）：
//   W 0x833ef0c C1 ctor（唯一动作：call Reset）
//   T 0x8336f60 Reset
//   T 0x8336f6e EquipArtifact(Inven_Item const*)
//   T 0x83336fc8 GetExperienceAmount
//   T 0x83337012 GetCreatureMinimumLevel
// 无 dtor。报告：docs/class_func_reports/user_creature__CArtifact/。
// ============================================================================
#include "CInventory.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "Inven_Item.h"

namespace user_creature
{

#include "CArtifact/CArtifact.inc"
}

#include "CArtifact/Reset.inc"
}

#include "CArtifact/EquipArtifact.inc"
}

#include "CArtifact/GetCreatureMinimumLevel.inc"
}

#include "CArtifact/GetExperienceAmount.inc"
}

}  // namespace user_creature
