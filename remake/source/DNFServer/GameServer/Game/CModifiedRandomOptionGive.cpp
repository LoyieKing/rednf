// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CModifiedRandomOptionGive/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ORIG 翻译：CModifiedRandomOptionGive（见 CModifiedRandomOptionGive.h 头注释）
// 5 符号：C1/C2、get_option_type、get_option_group_choice、set。
// 派生 override 直返指针（引用 ABI = 地址），set 依 ORIG 序存两成员。
#include "CModifiedRandomOptionGive.h"

#include "CModifiedRandomOptionGive/CModifiedRandomOptionGive.inc"
}

std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >&
#include "CModifiedRandomOptionGive/get_option_type.inc"
}

std::map<unsigned long long, OptionGroupChoiceValue>&
#include "CModifiedRandomOptionGive/get_option_group_choice.inc"
}

#include "CModifiedRandomOptionGive/set.inc"
}
