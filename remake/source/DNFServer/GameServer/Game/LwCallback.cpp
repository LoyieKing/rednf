// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LwCallback/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// LwCallback.cpp —— LwCallback 类实现（仅 ORIG）
// 报告: docs/class_func_reports/LwCallback/{README,LwCallback,LwCallback_08ad3ab8,getCallback,setCallback,log}.md
// D1/D0 无报告，按 objdump 0x08ad447a/0x08ad44dc 取证：D1 = vptr 复位 + ~std::function
// + 异常路径 LogWriter::~LogWriter + __in_chrg 检查；D0 = D1 + operator delete。
// 全部符号与 ORIG mangled 一致（nm 核验）。

#include "LwCallback.h"

#include "LwCallback/LwCallback.inc"
}

#include "LwCallback/LwCallback_1.inc"
}

#include "LwCallback/dtor_LwCallback.inc"
}

#include "LwCallback/setCallback.inc"
}

#include "LwCallback/log.inc"
}

std::function<void (char const*, char const*, int, char const*)>
#include "LwCallback/getCallback.inc"
}
