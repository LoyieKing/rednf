// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CData_charac_expand/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ charac_expand::CData 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CBoosterGage.h 为准。
#include "CBoosterGage.h"

namespace charac_expand
{

#include "CData_charac_expand/CData.inc"
}

#include "CData_charac_expand/dtor_CData.inc"
}

#include "CData_charac_expand/alter.inc"
}

// ORIG _ZN13charac_expand5CData6isSaveEv W 0x832ae80：`movzbl 0x4(%eax); ret` 直返
// m_bIsSave 字节（无 test;setne 布尔归一）。bool 字段 + bool 返回时 4.4 同样直返
// （探针 /tmp/l11p/issave.cpp：bool 字段 isSave 体 = movzbl;ret，调用点 = test %al;je；
// char 返回则调用点物化 setne——P73）。签名 mangled 不含返回类型。
#include "CData_charac_expand/isSave.inc"
}

#include "CData_charac_expand/saveData.inc"
}

#include "CData_charac_expand/reset.inc"
}

}  // namespace charac_expand
