// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_MomijiEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 阿拉德红叶事件 Arad_MomijiEvent 实现（Game 域；Monitor/ 同名类为
// 另一独立模块，与此无关）。
// 报告：docs/class_func_reports/Arad_MomijiEvent/ 全部 6 个方法已读。
//   C1 0x0819e738 / D1 0x0819e754 / D0 0x0819e784 / StartEvent 0x0819e7a2 /
//   StartEvent(Word_Param) 0x0819e7f4（thunk）/ EndEvent 0x0819e80e。
// 注意：StartEvent()/EndEvent() 中 trace 在 SetEventFlag 之前（ORIG 汇编序）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit ⇒
// 本 TU 发射 `_GLOBAL__I__ZN16Arad_MomijiEventC2Ev`（ORIG 0x819e89f）；ctor 已是首强符号。
#include <iostream>

#include "Arad_MomijiEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x0819e738：仅基类构造 + vptr 置位。
#include "Arad_MomijiEvent/Arad_MomijiEvent.inc"
}

// ORIG 0x0819e754 / 0x0819e784：空体 + 虚析构由编译器生成同形。
#include "Arad_MomijiEvent/dtor_Arad_MomijiEvent.inc"
}

// ORIG 0x0819e7a2：cMyTrace(PRETTY, 0x14, 0)("[MOMIJI_EVENT] Event Start.")
// 在前，SetEventFlag(true) 在后。
#include "Arad_MomijiEvent/StartEvent.inc"
}

// ORIG 0x0819e7f4（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
#include "Arad_MomijiEvent/StartEvent_1.inc"
}

// ORIG 0x0819e80e：cMyTrace(PRETTY, 0x1f, 0)("[MOMIJI_EVENT] Event End.")
// 在前，SetEventFlag(false) 在后。
#include "Arad_MomijiEvent/EndEvent.inc"
}
