// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CGoldCardBlankItemEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 金卡空白物品事件 CGoldCardBlankItemEvent 实现。
// 报告：docs/class_func_reports/CGoldCardBlankItemEvent/ 全部 6 个方法
// （CGoldCardBlankItemEvent / ~CGoldCardBlankItemEvent /
//  ~CGoldCardBlankItemEvent_0811ce70 / StartEvent / StartEvent_0811ce94 /
//  EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN23CGoldCardBlankItemEventC2Ev`（ORIG 0x811cf77）；ctor 已是首强符号。
#include <iostream>

#include "CGoldCardBlankItemEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811ce24：仅基类构造 + vptr 置位（无自有成员）。
#include "CGoldCardBlankItemEvent/CGoldCardBlankItemEvent.inc"
}

// ORIG 0x0811ce40 / 0x0811ce70：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CGoldCardBlankItemEvent/dtor_CGoldCardBlankItemEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ce8e：空体（仅 push/mov/pop/ret）。
#include "CGoldCardBlankItemEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ce94：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x18,0)("Gold Card Blank Item Event Start")。
#include "CGoldCardBlankItemEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cee6：SetEventFlag(false) +
// cMyTrace(__FUNCTION__,0x1e,0)("Gold Card Blank Item Event End")。
#include "CGoldCardBlankItemEvent/EndEvent.inc"
}
