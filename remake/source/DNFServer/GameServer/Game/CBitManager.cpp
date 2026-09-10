// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBitManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [b28d-38 判词] CBitManager::ShiftRight 0x0831c1.. DIFF：p167 CLEAR 0/0（无调用），
//  残差 N87（4.4 变长移位恒插 and $0x1f 掩码）+ N4（lea/shl 折叠）+ 帧槽差。
// CBitManager.cpp —— CBitManager 位流工具实现（ORIG 0x0831c09c..0x0831c3af）
//
// 语义来源：docs/class_func_reports/CBitManager/ 各 md 的 ## 汇编 + ## 反编译 C。
//
// 本 TU 自 WindowsPackFiles.cpp 拆出（batch-23 O_MiscC idx90，Main 批准）：
// ORIG 里 CBitManager 全族符号连续落在 0x0831c09c..0x0831c3af，而
// CObfuscator 在 0x08585888..，两者地址分段 ⇒ 原属不同编译单元。留在同一
// TU 内会引入与 ORIG 的人为偏差：4.4.7 见到同 TU 内 `CBitManager::~CBitManager()
// {}` 空体即推导 nothrow，从而对 CObfuscator::CObfuscator()（0x08585888）里的
// 临时 CBitManager 不发 EH landing pad（14 vs ORIG 29 insns）；拆出后
// WindowsPackFiles.cpp 只见声明，实测逐条复现 ORIG 29 条形（含 +0x2b..+0x41
// landing pad 与 -0x18(%ebp) 槽）。
// C++98 / 32 位 / O0；字段均为具名成员，无偏移宏。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CBitManagerC2Ev`（ORIG 0x831c3af）。
#include <iostream>

#include "WindowsPackFiles.h"

#include <cstdlib>
#include <cstring>

// ===========================================================================
// CBitManager
// ===========================================================================
#include "CBitManager/CBitManager.inc"
}                                                // 0x0831c09c

#include "CBitManager/dtor_CBitManager.inc"
}                                                // 0x0831c0c8 空析构

#include "CBitManager/SetAll.inc"
}

#include "CBitManager/ReadData.inc"
}

#include "CBitManager/GetBitFromLSB.inc"
}

// [N 类标注] 本函数与 ShiftLeft（0x0831c266）各 we-bigger 4（57/61、58/62）：
// 逐块比对确认差异全在**变长移位的计数掩码**——4.1 对 `shr/shl %cl` 不插
// `and $0x1f`（ORIG 侧仅 `sub 0xc(%ebp),%edx` 后直接 `shl %cl`），4.4 对每一处
// 变长移位一律前置 `and $0x1f,%edx`（实测各 3 处），另 ORIG 多一条重复
// `movl $0x0,-0x10(%ebp)` 循环量 init。源码侧 `bits & 0x1f` 已按 ORIG 语义保留，
// 掩码条数不可控 ⇒ 范式库 N 类「跨版本 变长移位计数掩码」，语义等价，勿再耗轮次。
#include "CBitManager/ShiftRight.inc"
}

#include "CBitManager/ShiftLeft.inc"
}

#include "CBitManager/GetBit.inc"
}
