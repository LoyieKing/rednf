// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSimpleLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CSimpleLog 实现 — ORIG 符号对齐
// 报告: docs/class_func_reports/CSimpleLog/ 及 docs/class_func_reports/CSimpleLog__DateTime/
// 全部函数已完整阅读 ## 汇编 + ## 反编译 C。
#include "CSimpleLog.h"

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>

// _ZN10CSimpleLog8DateTimeC1Ev / C2Ev @ 0x0854fa02
#include "CSimpleLog/DateTime.inc"
}

// _ZN10CSimpleLog8DateTime7DiffDayERS0_ @ 0x0854ff62 (W, 头内联; 此处 strong 定义)
#include "CSimpleLog/DiffDay.inc"
}

// _ZN10CSimpleLogC1Ev / C2Ev @ 0x0854fa88
#include "CSimpleLog/CSimpleLog.inc"
}

// _ZN10CSimpleLogD1Ev / D2Ev @ 0x0854faa8 (非虚: ORIG 无 vtable)
#include "CSimpleLog/dtor_CSimpleLog.inc"
}

// _ZN10CSimpleLog9CloseFileEv @ 0x0854fb06
#include "CSimpleLog/CloseFile.inc"
}

// _ZN10CSimpleLog8OpenFileEPKc @ 0x0854fabc
#include "CSimpleLog/OpenFile.inc"
}

// _ZN10CSimpleLog13CreateLogFileERNS_8DateTimeE @ 0x0854fb32
#include "CSimpleLog/CreateLogFile.inc"
}

// _ZN10CSimpleLog13CreateLogFileEPKc @ 0x0854fbba
#include "CSimpleLog/CreateLogFile_1.inc"
}

// _ZN10CSimpleLog9_WriteLogEPKc @ 0x0854fbfc
#include "CSimpleLog/_WriteLog.inc"
}

// _ZN10CSimpleLog8WriteLogEPKcz @ 0x0854fcb8
#include "CSimpleLog/WriteLog.inc"
}
