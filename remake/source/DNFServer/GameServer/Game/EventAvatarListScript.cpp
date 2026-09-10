// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventAvatarListScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventAvatarListScript 实现。
// ORIG：C1 W 0x8372e10（set 构造 + clear()）；clear W 0x8372e52（set::clear）。
#include "EventAvatarListScript.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；isfindEventAvatar() 前移为本 TU 首个
// 强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZN21EventAvatarListScript17isfindEventAvatarEi`
// （ORIG 0x896239d）。零语义/布局改动。
#include <iostream>

#include "EventAvatarListScript/isfindEventAvatar.inc"
}

#include "EventAvatarListScript/EventAvatarListScript.inc"
}

#include "EventAvatarListScript/dtor_EventAvatarListScript.inc"

#include "EventAvatarListScript/clear.inc"
}

