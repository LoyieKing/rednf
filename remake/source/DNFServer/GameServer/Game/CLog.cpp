// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CLog 全局日志单例实现。
// 报告：docs/class_func_reports/CLog/（17 函数全量转写；setLogWriterWired 无报告，
// 按 nm 0x0854e3db objdump 转写）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN4CLog5this_E`（ORIG 0x82a3bd9）；类静态数据成员定义为首强符号。
#include <iostream>

#include <cstdarg>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "CLog.h"
#include "LogManager.h"
#include "LwServerWriter.h"

// ORIG 0x0854e3db（t，_ZL17setLogWriterWiredv，文件级 static）：CLog 构造尾部调用。
// 注册两个 LwServerWriter：index 0 ← ENUM_LOG_TYPE_2、index 1 ← ENUM_LOG_TYPE_1。
// 必须为文件级 static（非匿名 namespace）以匹配 ORIG _ZL 符号（P138 shared_ptr 同构）。
static void setLogWriterWired()
{
    std::shared_ptr<LwServerWriter> w0(new LwServerWriter(ENUM_LOG_TYPE_2));
    LogManager::setLogWriter(0, std::shared_ptr<LogWriter>(w0));
    std::shared_ptr<LwServerWriter> w1(new LwServerWriter(ENUM_LOG_TYPE_1));
    LogManager::setLogWriter(1, std::shared_ptr<LogWriter>(w1));
}

// ORIG CLog::CLog 0x0854e558：10 个 Mutex 子对象（m_mutexes）默认构造；
// 链式清零 8 个 FILE* 句柄（+0xf0..+0x10f，含 +0x10c）；+0x250=0；
// memset(m_logFile+0x258, 0, 0x100)；memset(m_initFile+0x110, 0, 0x100)；
// [GI-ioinit L21] 键名符号前移（P327）：ORIG 0x82a3bd9（_ZN4CLog5this_E，B 型）。
CLog* CLog::this_ = 0;  // ORIG _ZN4CLog5this_E B @0x0940bd60

// setLogWriterWired()。
#include "CLog/CLog.inc"
}

// ORIG CLog::~CLog 0x0854e69c：逐个关非 stdout 句柄（0xf0/0xf4/0xf8/0x10c
// 关后置 0；0xfc/0x100/0x108 仅关不置 0；0x104 漏关——ORIG 行为照抄）；
// m_mutexes 由编译器隐式析构（循环）。
#include "CLog/dtor_CLog.inc"
}

// ORIG 单例为全局指针 0x940bd60（new 构造 / destroyInstance delete）。
// ORIG instance 0x0817dfec（0xa 字节）：直接返回全局指针（构造由 createInstance 完成）。
#include "CLog/instance.inc"
}

// ORIG _ZN4CLog15destroyInstanceEv（0x82a3cfd）：非空检查后 delete（delete
// 内部自带二次空检查）；全局指针无条件置 0。
#include "CLog/destroyInstance.inc"
}

// ORIG _ZN4CLog14createInstanceEv（0x82a3cb3，W）：new(0x358) 构造并存全局。
#include "CLog/createInstance.inc"
}

// ORIG initiate 0x0854e88c：
//   stat(name) 失败 → mkdir(name, 0x1fd)；仍失败 printf 后 return。
//   +0x250=flag、+0x254=1；m_logFile(+0x258) ← path（空则 "Log" 4 字节）；
//   8 个 TimeLog_@+0x210..+0x248 逐个 setTime；name 为空 → 句柄置 stdout
//   （0xf0/0xf4/0xf8/0xfc/0x100/0x108/0x10c，无 0x104——ORIG 照抄）；
//   否则 strcpy m_initFile(+0x110) 并 8 次 openLogFile（history 用 +0x254 flag）。
#include "CLog/initiate.inc"
}

// ORIG openLogFile 0x0854ec70：m_logFile(+0x258) 前缀 + 年月日（flag 非 0 加
// "-%02d" 小时）sprintf 拼名；strncpy/strncat 到 m_initFile(+0x110) 目录下；
// fopen "ab"；失败回退 stdout。
#include "CLog/openLogFile.inc"
}

// ORIG checkDay 0x0854f594：flag==0 仅比较日；flag!=0 且 (last->m_day !=
// t.m_day || (char)last->m_hour + flag <= (char)t.m_hour) 时 memcpy 8 字节、
// ORIG checkDay 0x0854f594：flag!=0 且 (last->m_day != t.m_day ||
// t.m_hour >= last->m_hour + flag) 时 memcpy 8 字节、fclose、openLogFile 重开；
// flag==0 仅比较日。恒返回 true。
#include "CLog/checkDay.inc"
}

// ORIG logToFile 0x0854edb0：this/句柄空检查；非 stdout 先 checkDay 日切；
// vsprintf + 可选 "[hh:mm:ss]" 前缀 + fprintf("%s\r\n") + fflush；
// bPrint 时同步 printf；返回 0/1。
#include "CLog/logToFile.inc"
}

// ORIG logToInitFile 0x0854ef56：memset 双缓冲 + setTime + checkDay 日切；
// vsprintf；格式串变量化（bTimestamp ? "\r\n%s" : "%s"，printf 与 fprintf 共用）；
// bPrint 时行首加时间戳；返回 0/1。
#include "CLog/logToInitFile.inc"
}

// ORIG logNotice 0x0854f162：logToFile(+0xf0, +0x210, false, ".log", fmt, va, true)。
#include "CLog/logNotice.inc"
}

// ORIG logError 0x0854f1c2：logToFile(+0xf4, +0x218, false, ".error", ..., true)。
#include "CLog/logError.inc"
}

// ORIG logDebug 0x0854f40a：仅 return 1。
#include "CLog/logDebug.inc"
}

// ORIG logConsole 0x0854f414：this 空返回 0；setTime + vsprintf +
// "[hh:mm:ss]" 前缀 + printf("%s\r\n")，返回 1。
#include "CLog/logConsole.inc"
}

// ORIG logMoney 0x0854f282：logToFile(+0xfc, +0x228, false, ".money", ..., true)。
#include "CLog/logMoney.inc"
}

// ORIG logCritical 0x0854f222：logToFile(+0x104, +0x238, false, ".cri", ..., true)。
#include "CLog/logCritical.inc"
}

// ORIG logSnapShot 0x0854f4c0：logToFile(+0x10c, +0x248, false, ".snap", ..., false)。
#include "CLog/logSnapShot.inc"
}

// ORIG logInit 0x0854f520：logToInitFile(+0x108, +0x240, ".init", fmt, va,
// bForceOpen, bAppend)。
#include "CLog/logInit.inc"
}

// ORIG logHistory 0x0854f2e2：this/历史句柄(+0x100) 空返回 0；非 stdout 时
// setTime + checkDay(+0x230, +0x100, ".history", +0x254) 日切；
// vsprintf + strlen 空循环 + fprintf("%s\r\n") + fflush，返回 1。
#include "CLog/logHistory.inc"
}

// ORIG getTime 0x0854f692：time(0) + localtime_r（P43：经返回指针访问 tm 字段）。
#include "CLog/getTime.inc"
}
// ORIG TimeLog_::setTime 0x0854e228（本 TU 依赖，报告 docs/class_func_reports/
// TimeLog_/setTime.md）：与 getTime 同构，写 this 字段。
#include "CLog/setTime.inc"
}
