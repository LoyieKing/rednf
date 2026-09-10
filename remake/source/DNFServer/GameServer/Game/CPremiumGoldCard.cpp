// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPremiumGoldCard/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 黄金卡充值事件 CPremiumGoldCard 实现。
// 报告缺失（全部 6 方法为 ORIG W 弱符号，objdump 转写）：
//   C1 0x0811c432 / D1 0x0811c44e / D0 0x0811c47e /
//   StartEvent 0x0811c49c / StartEvent(Word_Param) 0x0811c4ee / EndEvent 0x0811c508。
#include "CPremiumGoldCard.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c432：仅基类构造 + vptr 置位。
#include "CPremiumGoldCard/CPremiumGoldCard.inc"
}

// ORIG 0x0811c44e / 0x0811c47e：空体 + 虚析构由编译器生成同形。
#include "CPremiumGoldCard/dtor_CPremiumGoldCard.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c49c：置事件标志并打日志（__PRETTY_FUNCTION__，line 0x10）。
#include "CPremiumGoldCard/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c4ee：vtable+0x8 直调 StartEvent()（转发无参版本）。
#include "CPremiumGoldCard/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c508：清事件标志并打日志（__PRETTY_FUNCTION__，line 0x1a）。
#include "CPremiumGoldCard/EndEvent.inc"
}
