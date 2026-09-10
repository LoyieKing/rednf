// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STDeadTowerDialog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// STDeadTowerDialog 实现。
// 证据: objdump 0x0891b12c..0x0891b18a（ctor/dtor/clear）与
//   0x08929428..0x089294d8（copy ctor/operator=）；类报告缺失, nm 定址转写。
#include "STDeadTowerDialog.h"

// 全局 C++ 权威签名（DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）。
// 不 include DNFLexWrapper.h：该头经 DNFLex.h 引入 <iostream>，会给本 TU 带上
// std::ios_base::Init（__ioinit）动态初始化与 `_GLOBAL__I_` 键；ORIG 本 TU 无 GI
// （orig_tu_names.tsv 513 条无 STDeadTowerDialog.cpp）。签名与 DNFLexWrapper.h 一致。
#include "STDeadTowerDialog/loadRDARScriptFile.inc"
#include "STDeadTowerDialog/ScanType.inc"
#include "STDeadTowerDialog/ScanInt.inc"
#include "STDeadTowerDialog/ScanStr.inc"
#include "STDeadTowerDialog/importDeadTowerDialogScript.inc"
}

#include "STDeadTowerDialog/STDeadTowerDialog.inc"
}

#include "STDeadTowerDialog/dtor_STDeadTowerDialog.inc"
}

#include "STDeadTowerDialog/clear.inc"
}

#include "STDeadTowerDialog/STDeadTowerDialog_1.inc"
}

STDeadTowerDialog& STDeadTowerDialog::operator=(STDeadTowerDialog const& r)
{
    m_index = r.m_index;
    m_str = r.m_str;
    m_flag1 = r.m_flag1;
    m_flag2 = r.m_flag2;
    m_flag3 = r.m_flag3;
    return *this;
}
