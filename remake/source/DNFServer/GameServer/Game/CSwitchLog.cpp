// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSwitchLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CSwitchLog（频道/线路切换日志，G2-4 批次）
// 参照 docs/class_func_reports/CSwitchLog.md。
// ORIG 符号：
//   _ZN10CSwitchLogC1EPKciii / C2（0x0854fd3c）
//   _ZN10CSwitchLog15SetLogWriteFlagEi（0x0854fd18，静态）
//   _ZN10CSwitchLogclEPKcz（0x0854fd64）
// 跨类调用：CLog::instance/logNotice/logError/logDebug/logConsole/logMoney/
// logCritical、Guard<Mutex>（真实符号，定义在其它 TU）。
// ============================================================================

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "CLog.h"
#include "CSwitchLog.h"   // 权威声明（本 TU 唯一实现）

char CSwitchLog::log_write_[1];   // ORIG 0x0943df78，1 字节
#include "CSwitchLog/CSwitchLog.inc"
}

#include "CSwitchLog/SetLogWriteFlag.inc"
}

// [b28d-23 N·N82] ⇒ N。ORIG 0x0854fd64
#include "CSwitchLog/operator.inc"
}
