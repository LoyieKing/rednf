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
// setLogWriterWired()。
CLog::CLog()
{
    m_logFileHistory = m_logFileNotice = m_logFileError = m_logFileDebug =
        m_logFileMoney = m_logFileCri = m_logFileInit = 0;
    m_logFileSnap = 0;
    m_flag = 0;
    std::memset(m_logFile, 0, sizeof(m_logFile));
    std::memset(m_initFile, 0, sizeof(m_initFile));
    setLogWriterWired();
}

// ORIG CLog::~CLog 0x0854e69c：逐个关非 stdout 句柄（0xf0/0xf4/0xf8/0x10c
// 关后置 0；0xfc/0x100/0x108 仅关不置 0；0x104 漏关——ORIG 行为照抄）；
// m_mutexes 由编译器隐式析构（循环）。
CLog::~CLog()
{
    if (m_logFileNotice != 0)
    {
        if (m_logFileNotice != stdout)
            std::fclose(m_logFileNotice);
        m_logFileNotice = 0;
    }
    if (m_logFileError != 0)
    {
        if (m_logFileError != stdout)
            std::fclose(m_logFileError);
        m_logFileError = 0;
    }
    if (m_logFileDebug != 0)
    {
        if (m_logFileDebug != stdout)
            std::fclose(m_logFileDebug);
        m_logFileDebug = 0;
    }
    if (m_logFileMoney != 0 && m_logFileMoney != stdout)
        std::fclose(m_logFileMoney);
    if (m_logFileHistory != 0 && m_logFileHistory != stdout)
        std::fclose(m_logFileHistory);
    if (m_logFileInit != 0 && m_logFileInit != stdout)
        std::fclose(m_logFileInit);
    if (m_logFileSnap != 0)
    {
        if (m_logFileSnap != stdout)
            std::fclose(m_logFileSnap);
        m_logFileSnap = 0;
    }
}

CLog* CLog::this_ = 0;  // ORIG _ZN4CLog5this_E B @0x0940bd60

// ORIG 单例为全局指针 0x940bd60（new 构造 / destroyInstance delete）。
// ORIG instance 0x0817dfec（0xa 字节）：直接返回全局指针（构造由 createInstance 完成）。
CLog* CLog::instance()
{
    return this_;
}

// ORIG _ZN4CLog15destroyInstanceEv（0x82a3cfd）：非空检查后 delete（delete
// 内部自带二次空检查）；全局指针无条件置 0。
void CLog::destroyInstance()
{
    if (this_ != 0)
	    {
		        delete this_;
	    }
	this_ = 0;
}

// ORIG _ZN4CLog14createInstanceEv（0x82a3cb3，W）：new(0x358) 构造并存全局。
void CLog::createInstance()
{
    this_ = new CLog();
}

// ORIG initiate 0x0854e88c：
//   stat(name) 失败 → mkdir(name, 0x1fd)；仍失败 printf 后 return。
//   +0x250=flag、+0x254=1；m_logFile(+0x258) ← path（空则 "Log" 4 字节）；
//   8 个 TimeLog_@+0x210..+0x248 逐个 setTime；name 为空 → 句柄置 stdout
//   （0xf0/0xf4/0xf8/0xfc/0x100/0x108/0x10c，无 0x104——ORIG 照抄）；
//   否则 strcpy m_initFile(+0x110) 并 8 次 openLogFile（history 用 +0x254 flag）。
void CLog::initiate(char* name, char* path, int flag)
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

// ORIG openLogFile 0x0854ec70：m_logFile(+0x258) 前缀 + 年月日（flag 非 0 加
// "-%02d" 小时）sprintf 拼名；strncpy/strncat 到 m_initFile(+0x110) 目录下；
// fopen "ab"；失败回退 stdout。
void CLog::openLogFile(TimeLog_ t, FILE*& out, char* ext, int flag)
{
    char path[4096];
    char buf[4096];
    if (flag != 0)
    {
        std::sprintf(buf, "%s%04d%02d%02d-%02d", m_logFile,
                     (int)(short)t.m_year, (int)t.m_mon, (int)t.m_day,
                     (int)t.m_hour);
    }
    else
    {
        std::sprintf(buf, "%s%04d%02d%02d", m_logFile,
                     (int)(short)t.m_year, (int)t.m_mon, (int)t.m_day);
    }
    std::strncpy(path, m_initFile, 0x1000);
    std::strncat(path, buf, 0x1000);
    std::strncat(path, ext, 0x1000);
    if ((out = std::fopen(path, "ab")) == 0)
        out = stdout;
}

// ORIG checkDay 0x0854f594：flag==0 仅比较日；flag!=0 且 (last->m_day !=
// t.m_day || (char)last->m_hour + flag <= (char)t.m_hour) 时 memcpy 8 字节、
// ORIG checkDay 0x0854f594：flag!=0 且 (last->m_day != t.m_day ||
// t.m_hour >= last->m_hour + flag) 时 memcpy 8 字节、fclose、openLogFile 重开；
// flag==0 仅比较日。恒返回 true。
bool CLog::checkDay(TimeLog_ t, TimeLog_* last, FILE*& out, char* ext, int flag)
{
    if (flag != 0)
    {
        if (last->m_day != t.m_day || t.m_hour >= last->m_hour + flag)
        {
            std::memcpy(last, &t, 8);
            std::fclose(out);
            openLogFile(t, out, ext, flag);
        }
    }
    else
    {
        if (last->m_day != t.m_day)
        {
            std::memcpy(last, &t, 8);
            std::fclose(out);
            openLogFile(t, out, ext, flag);
        }
    }
    return true;
}

// ORIG logToFile 0x0854edb0：this/句柄空检查；非 stdout 先 checkDay 日切；
// vsprintf + 可选 "[hh:mm:ss]" 前缀 + fprintf("%s\r\n") + fflush；
// bPrint 时同步 printf；返回 0/1。
int CLog::logToFile(FILE*& out, TimeLog_& last, bool bPrint, char* ext,
                    char const* fmt, char* va, bool bTimestamp)
{
    if (this == 0)
        return 0;
    if (out == 0)
        return 0;
    TimeLog_ now;
    now.setTime();
    if (out != stdout)
    {
        if (!checkDay(now, &last, out, ext, m_flag))
            return 0;
    }
    char line[4096];
    char body[4096];
    vsprintf(body, fmt, va);
    if (bTimestamp)
    {
        std::sprintf(line, "[%02d:%02d:%02d] %s", (int)now.m_hour,
                     (int)now.m_min, (int)now.m_sec, body);
        std::fprintf(out, "%s\r\n", line);
    }
    else
    {
        std::fprintf(out, "%s\r\n", body);
    }
    std::fflush(out);
    if (bPrint)
    {
        if (bTimestamp)
            std::printf("%s\r\n", line);
        else
            std::printf("%s\r\n", body);
    }
    return 1;
}

// ORIG logToInitFile 0x0854ef56：memset 双缓冲 + setTime + checkDay 日切；
// vsprintf；格式串变量化（bTimestamp ? "\r\n%s" : "%s"，printf 与 fprintf 共用）；
// bPrint 时行首加时间戳；返回 0/1。
int CLog::logToInitFile(FILE*& out, TimeLog_& last, char* ext,
                        char const* fmt, char* va, bool bPrint, bool bTimestamp)
{
    if (this == 0)
        return 0;
    if (out == 0)
        return 0;
    char line[0x1000];
    char body[0x1000];
    std::memset(line, 0, sizeof(line));
    std::memset(body, 0, sizeof(body));
    TimeLog_ now;
    now.setTime();
    if (out != stdout)
    {
        if (!checkDay(now, &last, out, ext, m_flag))
            return 0;
    }
    vsprintf(body, fmt, va);
    char fmtStr[16] = { 0 };
    if (bTimestamp)
        std::memcpy(fmtStr, "%s\r\n", 5);   // ORIG 0x8c9ede4（后缀 CRLF）
    else
        std::memcpy(fmtStr, "%s", 3);         // ORIG 0x8c9ed4b
    std::printf(fmtStr, body);
    if (bPrint)
    {
        std::sprintf(line, "[%02d:%02d:%02d] %s", (int)now.m_hour,
                     (int)now.m_min, (int)now.m_sec, body);
    }
    else
    {
        std::strcpy(line, body);
    }
    std::fprintf(out, fmtStr, line);
    std::fflush(out);
    return 1;
}

// ORIG logNotice 0x0854f162：logToFile(+0xf0, +0x210, false, ".log", fmt, va, true)。
unsigned char CLog::logNotice(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileNotice, m_logTime[0], false, ".log", fmt,
                                  (char*)ap, true);
    return ret;
}

// ORIG logError 0x0854f1c2：logToFile(+0xf4, +0x218, false, ".error", ..., true)。
unsigned char CLog::logError(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileError, m_logTime[1], false, ".error", fmt,
                                  (char*)ap, true);
    return ret;
}

// ORIG logDebug 0x0854f40a：仅 return 1。
int CLog::logDebug(const char*, ...)
{
    return 1;
}

// ORIG logConsole 0x0854f414：this 空返回 0；setTime + vsprintf +
// "[hh:mm:ss]" 前缀 + printf("%s\r\n")，返回 1。
unsigned char CLog::logConsole(const char* fmt, ...)
{
    if (this == 0)
        return 0;
    TimeLog_ now;
    now.setTime();
    va_list ap;
    va_start(ap, fmt);
    char line[4096];
    char body[4096];
    vsprintf(body, fmt, ap);
    std::sprintf(line, "[%02d:%02d:%02d] %s", (int)now.m_hour,
                 (int)now.m_min, (int)now.m_sec, body);
    std::printf("%s\r\n", line);
    return 1;
}

// ORIG logMoney 0x0854f282：logToFile(+0xfc, +0x228, false, ".money", ..., true)。
unsigned char CLog::logMoney(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileMoney, m_logTime[3], false, ".money", fmt,
                                  (char*)ap, true);
    return ret;
}

// ORIG logCritical 0x0854f222：logToFile(+0x104, +0x238, false, ".cri", ..., true)。
unsigned char CLog::logCritical(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileCri, m_logTime[5], false, ".cri", fmt,
                                  (char*)ap, true);
    return ret;
}

// ORIG logSnapShot 0x0854f4c0：logToFile(+0x10c, +0x248, false, ".snap", ..., false)。
unsigned char CLog::logSnapShot(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileSnap, m_logTime[7], false, ".snap", fmt,
                                  (char*)ap, false);
    return ret;
}

// ORIG logInit 0x0854f520：logToInitFile(+0x108, +0x240, ".init", fmt, va,
// bForceOpen, bAppend)。
unsigned char CLog::logInit(bool bForceOpen, bool bAppend, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToInitFile(m_logFileInit, m_logTime[6], ".init", fmt,
                                      (char*)ap, bForceOpen, bAppend);
    return ret;
}

// ORIG logHistory 0x0854f2e2：this/历史句柄(+0x100) 空返回 0；非 stdout 时
// setTime + checkDay(+0x230, +0x100, ".history", +0x254) 日切；
// vsprintf + strlen 空循环 + fprintf("%s\r\n") + fflush，返回 1。
int CLog::logHistory(const char* fmt, char* vaArgs)
{
    if (this == 0)
        return 0;
    if (m_logFileHistory == 0)
        return 0;
    if (m_logFileHistory != stdout)
    {
        TimeLog_ now;
        now.setTime();
        if (!checkDay(now, &m_logTime[4], m_logFileHistory, ".history",
                     m_initFlag))
            return 0;
    }
    char body[4096];
    vsprintf(body, fmt, vaArgs);
    int len = (int)std::strlen(body);
    int i;
    for (i = 0; i < len; ++i)
    {
    }
    std::fprintf(m_logFileHistory, "%s\r\n", body);
    std::fflush(m_logFileHistory);
    return 1;
}

// ORIG getTime 0x0854f692：time(0) + localtime_r（P43：经返回指针访问 tm 字段）。
void CLog::getTime(TimeLog_* t)
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
// ORIG TimeLog_::setTime 0x0854e228（本 TU 依赖，报告 docs/class_func_reports/
// TimeLog_/setTime.md）：与 getTime 同构，写 this 字段。
void TimeLog_::setTime()
{
    std::time_t now = std::time(0);
    std::tm lt;
    struct tm* p = localtime_r(&now, &lt);
    m_year = (short)(p->tm_year + 0x76c);
    m_mon = p->tm_mon + 1;
    m_day = p->tm_mday;
    m_hour = p->tm_hour;
    m_min = p->tm_min;
    m_sec = p->tm_sec;
}
