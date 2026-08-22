#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "CLog.h"

namespace {

// 时间戳前缀写入日志行：ORIG 各 log* 使用 "[%02d:%02d:%02d] %s\r\n" 与 m_logFile
// 回退到 stdout（openLogFile 在 fopen 失败时默认 stdout）。格式与 ORIG 汇编一致。
// 【推断】文件句柄成员在 CLog.h 中未建模，重建采用 ORIG 的 stdout 默认回退语义。
void logLine(const char* fmt, va_list ap)
{
    char body[4096];
    vsnprintf(body, sizeof(body), fmt, ap);

    std::time_t t = std::time(0);
    std::tm lt;
#if defined(_WIN32)
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    char line[8192];
    std::snprintf(line, sizeof(line), "[%02d:%02d:%02d] %s\r\n",
                  lt.tm_hour, lt.tm_min, lt.tm_sec, body);
    std::fputs(line, stdout);
    std::fflush(stdout);
}

} // namespace

CLog::CLog()
{
    // ORIG CLog::CLog（0x0854e558）：10 个 Mutex（m_mutexes，0x18/个）、
    // 标志区 0xf0..0x10c 清零、m_initFile(+0x110)/m_logFile(+0x258) 路径缓冲清零。
    // 【推断】m_mutexes 为裸缓冲，无法在此构造 Mutex 对象；按报告初始化其余成员。
    std::memset(m_logFlags, 0, sizeof(m_logFlags));
    std::memset(m_initFile, 0, sizeof(m_initFile));
    std::memset(m_logFile, 0, sizeof(m_logFile));
}

CLog* CLog::instance()
{
    static CLog log;
    return &log;
}

// logDebug：ORIG 仅为 return 1（无实际写入），保持空实现语义。
void CLog::logDebug(const char*, ...)
{
}

// logConsole：ORIG（0x0854f414）——vsnprintf + "[hh:mm:ss]" 前缀 + printf("%s\r\n")。
void CLog::logConsole(const char* fmt, ...)
{
    if (!fmt)
        return;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
}

// logNotice / logError / logMoney / logCritical：
// ORIG 各调 logToFile（0x0854edb0）写对应文件（".log"/".error"/".money"/".cri"），
// openLogFile 在 fopen 失败时回退 stdout，格式 "[%02d:%02d:%02d] %s\r\n"。
// 【推断】文件句柄成员未在 CLog.h 建模，统一采用 stdout 默认回退写入。
void CLog::logNotice(const char* fmt, ...)
{
    if (!fmt)
        return;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
}

void CLog::logError(const char* fmt, ...)
{
    if (!fmt)
        return;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
}

void CLog::logMoney(const char* fmt, ...)
{
    if (!fmt)
        return;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
}

void CLog::logCritical(const char* fmt, ...)
{
    if (!fmt)
        return;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
}
