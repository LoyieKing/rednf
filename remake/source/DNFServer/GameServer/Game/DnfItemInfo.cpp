// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DnfItemInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r DnfItemInfo 实现（OrigDataClears 批，2026-08-28）。
// ORIG：ctor W 0x84e910a / reset W 0x84e9094（objdump 逐指令转写）。
// 语义与 Guild/DNFGuild.cpp 的 Guild 域实现一致（同 mangled 名，Game 服务补齐）。
#include "DnfItemInfo.h"

#include "Inven_Item.h"   // ChronicleRandomOptionInfo（本文件唯一定义点）

// ORIG ctor 0x84e910a：UpgradeSeparateInfo C1@+0x2b（0x80cb332）、
// ReservedCapacity C1@+0x2c（0x80cb36c）由编译器成员构造自动生成，函数体仅调 reset()。
#include "DnfItemInfo/DnfItemInfo.inc"
}

// ORIG reset 0x84e9094：8 个标量字段逐条清零（movb/movl/movw 宽度照 ORIG），
// 随后 RandomOption::reset@+0x1d（0x80cb2d8）、UpgradeSeparateInfo::reset@+0x2b
// （0x80cb346）、ReservedCapacity::reset@+0x2c（0x80cb380）。
#include "DnfItemInfo/reset.inc"
}

// ORIG 0x822abea W：委托两 STEnchantedSkillOption::reset + m_field2 清零 +
// RandomOption::reset。定义置于本 TU（callee reset 均定义于 Inven_Item.cpp）：
// caller TU 不含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing 帧，
// 保持 ORIG 的 sub $0x18（NL4 实证）。
#include "DnfItemInfo/reset_1.inc"
}

// RandomOptionSeed::reset（ORIG W 0x080cb2cc，m_field=0）：定义原在
// Inven_Item.cpp，迁入本 TU（RandomOption::reset 的 callee 之一）——
// RandomOption::reset 所在 TU 不再含全部 callee 定义体 ⇒ 4.4 unit-at-a-time
// 不塌缩 outgoing 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
#include "DnfItemInfo/reset_2.inc"
}
