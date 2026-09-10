// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPcRoomCardBlankItemEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r PC 房卡片空白道具事件 CPcRoomCardBlankItemEvent 实现。
// 报告：docs/class_func_reports/CPcRoomCardBlankItemEvent/ 全部 6 个方法。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN25CPcRoomCardBlankItemEventC2Ev`（ORIG 0x826758f）；ctor 已是首强符号。
#include <iostream>

#include "CPcRoomCardBlankItemEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826743c：仅基类构造 + vptr 置位。
#include "CPcRoomCardBlankItemEvent/CPcRoomCardBlankItemEvent.inc"
}

// ORIG 0x08267458 / 0x08267488：空体 + 虚析构由编译器生成同形。
#include "CPcRoomCardBlankItemEvent/dtor_CPcRoomCardBlankItemEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x082674a6：空函数（push/mov/pop/ret）。
#include "CPcRoomCardBlankItemEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x082674ac：置事件标志并打日志（忽略 param）。
#include "CPcRoomCardBlankItemEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x082674fe：清事件标志并打日志。
#include "CPcRoomCardBlankItemEvent/EndEvent.inc"
}
