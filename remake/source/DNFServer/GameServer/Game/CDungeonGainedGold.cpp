// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDungeonGainedGold/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CDungeonGainedGold — 副本内金币累计（ORIG 0x850d4b6 / 0x84ec5d8 / 0x8697044..）。
// 权威参照 docs/class_func_reports/_lt_global_gt_.md 对应段。
// 布局：+0x00 unsigned int（ORIG 类尺寸 4；CUser.h 内嵌于 +0x8cf64）。
// 类定义唯一见 CDungeonGainedGold.h（CUser.h 值成员依赖此头）。
#include "CDungeonGainedGold.h"

// ORIG 0x869705c / 0x8697062：空体 ctor/dtor（CUser ctor 0x864e410 调用实证）。
#include "CDungeonGainedGold/CDungeonGainedGold.inc"
#include "CDungeonGainedGold/dtor_CDungeonGainedGold.inc"

#include "CDungeonGainedGold/incGainedGold.inc"
}

#include "CDungeonGainedGold/getGainedGold.inc"
}

#include "CDungeonGainedGold/setGainedGold.inc"
}

#include "CDungeonGainedGold/reset.inc"
}
