// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CLog/CLog.inc"


#include "CLog/dtor_CLog.inc"


#include "CLog/instance.inc"


#include "CLog/destroyInstance.inc"


#include "CLog/createInstance.inc"
{
    this_ = new CLog();
}

#include "CLog/initiate.inc"
{
    struct stat st;
    if (stat(name, &st) < 0)
    {
        if (mkdir(name, 0x1fd) < 0)
        {
            std::printf("Fail To Make Directory, PATH[%s]\n", name);
            return;
        }
    }
    m_flag = flag;
    m_initFlag = 1;
    if (path != 0)
        std::memcpy(m_logFile, path, 0x100);
    else
        std::memcpy(m_logFile, "Log", 4);
    m_logTime[0].setTime();
    m_logTime[1].setTime();
    m_logTime[2].setTime();
    m_logTime[3].setTime();
    m_logTime[4].setTime();
    m_logTime[5].setTime();
    m_logTime[6].setTime();
    m_logTime[7].setTime();
    if (name == 0)
    {
        m_logFileNotice = stdout;
        m_logFileError = stdout;
        m_logFileDebug = stdout;
        m_logFileMoney = stdout;
        m_logFileHistory = stdout;
        m_logFileInit = stdout;
        m_logFileSnap = stdout;
    }
    else
    {
        std::strcpy(m_initFile, name);
        openLogFile(m_logTime[0], m_logFileNotice, ".log", m_flag);
        openLogFile(m_logTime[1], m_logFileError, ".error", m_flag);
        openLogFile(m_logTime[2], m_logFileDebug, ".debug", m_flag);
        openLogFile(m_logTime[3], m_logFileMoney, ".money", m_flag);
        openLogFile(m_logTime[4], m_logFileHistory, ".history", m_initFlag);
        openLogFile(m_logTime[6], m_logFileInit, ".init", m_flag);
        openLogFile(m_logTime[5], m_logFileCri, ".cri", m_flag);
        openLogFile(m_logTime[7], m_logFileSnap, ".snap", m_flag);
    }
}

#include "CLog/openLogFile.inc"


#include "CLog/checkDay.inc"


#include "CLog/logToFile.inc"


#include "CLog/logToInitFile.inc"


#include "CLog/logNotice.inc"


#include "CLog/logError.inc"


#include "CLog/logDebug.inc"


#include "CLog/logConsole.inc"


#include "CLog/logMoney.inc"


#include "CLog/logCritical.inc"


#include "CLog/logSnapShot.inc"


#include "CLog/logInit.inc"


#include "CLog/logHistory.inc"


#include "CLog/getTime.inc"
{
    std::time_t now = std::time(0);
    std::tm lt;
    struct tm* p = localtime_r(&now, &lt);
    t->m_year = (short)(p->tm_year + 0x76c);
    t->m_mon = (unsigned char)((char)p->tm_mon + 1);
    t->m_day = (unsigned char)p->tm_mday;
    t->m_hour = (unsigned char)p->tm_hour;
    t->m_min = (unsigned char)p->tm_min;
    t->m_sec = (unsigned char)p->tm_sec;
}
// [SplitC 2026-09-11 batch4 · TU 迁移] TimeLog_::setTime 已迁出至 log.cpp（ORIG GI 键对齐）。
