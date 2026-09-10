// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CExceptLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CExceptLog —— 异常日志类 ORIG 实现（Wave5 续批）。
// 证据：docs/class_func_reports/CExceptLog/ 全文（4 函数 + 全局 ios init）。
// C1/C2 同址（普通类无虚基），一个 ctor 实现；_GLOBAL__I_ 为 iostream 头
// 自动生成的 ios_base::Init 静态初始化。
// 注：fPrintTextFile 以 this 对象内存作 vsprintf 缓冲（ORIG 原行为）。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10CExceptLog13PrintTextFileEPcS0_`（ORIG 0x846d7e5）；键名符号为
// PrintTextFile（非 ctor），待移序为 TU 首强符号。
#include <iostream>

#include "CExceptLog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0846d640。
#include "CExceptLog/PrintTextFile.inc"
}

// ORIG ctor @0x0846d736：memset(this,0,2000) → Set_Time_String(buf) →
// fPrintTextFile("Except.txt","%s\n%s\n",buf,str.c_str())。
#include "CExceptLog/CExceptLog.inc"
}

// ORIG @0x0846d682：vsprintf 写入 (char*)this，再 PrintTextFile(name,(char*)this)。
#include "CExceptLog/fPrintTextFile.inc"
}

// ORIG @0x0846d6c2：time(&t) → localtime_r(&t,&tm) → sprintf 时间串。
#include "CExceptLog/Set_Time_String.inc"
}
