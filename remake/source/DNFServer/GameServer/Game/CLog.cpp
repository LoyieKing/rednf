#include <cstdarg>
#include <sys/stat.h>
#include <sys/types.h>
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
namespace
{
CLog* s_logInstance = 0;  // ORIG 全局 0x940bd60
}  // namespace

// ORIG 单例为全局指针 0x940bd60（new 构造 / destroyInstance delete）。
CLog* CLog::instance()
{
    if (!s_logInstance)
        s_logInstance = new CLog();
    return s_logInstance;
}

// ORIG _ZN4CLog15destroyInstanceEv（0x82a3cfd，0x3a 字节）：delete 后指针置空。
void CLog::destroyInstance()
{
    if (s_logInstance)
    {
        delete s_logInstance;
        s_logInstance = 0;
    }
}

// ORIG _ZN4CLog14createInstanceEv（0x82a3cb3，W）：new(0x358) 构造并存全局。
void CLog::createInstance()
{
    s_logInstance = new CLog();
}

// ORIG _ZN4CLog8initiateEPcS0_i（0x854e88c，T，0x3e3 字节）：
//   1) stat(param2 目录)；失败则 mkdir(0x1fd)，仍失败 printf 后返回；
//   2) this+0x250=param4(flag)、+0x254=1；
//   3) m_logFile(+0x258) ← param3（空则 "Log/"）；
//   4) 8 个 TimeLog_ setTime；param2 为空时各文件句柄置 stdout，
//      否则 strcpy m_initFile(+0x110) 并逐个 openLogFile。
// TimeLog_/文件句柄成员未建模 —「推断」骨架：保留目录创建与路径记录语义，
// 文件句柄沿用本 TU 的 stdout 回退。TODO(G2)。
void CLog::initiate(char* name, char* path, int flag)
{
    struct stat st;
    if (stat(path, &st) != 0 && mkdir(path, 0x1fd) != 0)
    {
        std::printf("[CLog] initiate: mkdir(%s) failed\n", path);
        return;
    }
    m_logFlags[0] = flag;                       // ORIG +0x250（推断映射）
    if (path)
        std::strncpy(m_initFile, path, sizeof(m_initFile) - 1);
    const char* logName = (name != 0) ? name : "Log/";
    std::strncpy(m_logFile, logName, sizeof(m_logFile) - 1);
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

// logSnapShot：ORIG（0x0854f4c0）调 logToFile(m_logFile+0x248 句柄区, TimeLog@+0x10c,
// fmt, va, false, ".snap")。重建无文件句柄建模，按 stdout 回退语义写入。
bool CLog::logSnapShot(const char* fmt, ...)
{
    if (!fmt)
        return false;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
    return true;
}

// logInit：ORIG（0x0854f520）调 logToInitFile(m_initFile+0x108 句柄, TimeLog@+0x240,
// fmt, va, bForceOpen, bAppend)。重建采用 stdout 回退语义。
bool CLog::logInit(bool bForceOpen, bool bAppend, const char* fmt, ...)
{
    (void)bForceOpen;
    (void)bAppend;  // 【推断】文件打开策略未建模
    if (!fmt)
        return false;
    va_list ap;
    va_start(ap, fmt);
    logLine(fmt, ap);
    va_end(ap);
    return true;
}

// logHistory：ORIG（0x0854f2e2）—— this==0 或历史句柄(+0x100)==0 → 0；
// 非 stdout 句柄走 checkDay 日切（".history"）；vsprintf + fprintf("%s\r\n")
// + fflush，返回 1。无时间戳前缀（与 logNotice 系不同）。
// 【推断】历史文件句柄成员未在 CLog.h 建模，采用 stdout 回退写入。
bool CLog::logHistory(const char* fmt, char* vaArgs)
{
    if (this == 0 || !fmt)
        return false;

    char body[4096];
    vsnprintf(body, sizeof(body), fmt, (va_list)vaArgs);
    std::fprintf(stdout, "%s\r\n", body);
    std::fflush(stdout);
    return true;
}
