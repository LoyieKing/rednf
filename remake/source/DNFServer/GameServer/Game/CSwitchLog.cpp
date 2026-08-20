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

class Mutex;

template <class T>
class Guard
{
public:
    Guard(T* p);
    ~Guard();
};

class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int level, int flagIndex);
    void operator()(const char* fmt, ...);
    static void SetLogWriteFlag(int flag);

private:
    const char* m_name;    // +0x00
    int m_line;            // +0x04
    int m_level;           // +0x08
    int m_flagIndex;       // +0x0c

    static char log_write_[1];   // ORIG 0x0943df78，1 字节
};

char CSwitchLog::log_write_[1];

CSwitchLog::CSwitchLog(const char* name, int line, int level, int flagIndex)
{
    m_name = name;
    m_line = line;
    m_level = level;
    m_flagIndex = flagIndex;
}

void CSwitchLog::SetLogWriteFlag(int flag)
{
    if (flag == 0)
        log_write_[0] = 0;
    else if (flag == 1)
        log_write_[0] = 1;
}

void CSwitchLog::operator()(const char* fmt, ...)
{
    if (log_write_[m_flagIndex]) {
        char buf[4096];
        sprintf(buf, "%s(%d): ", m_name, m_line);
        va_list ap;
        va_start(ap, fmt);
        int len = strlen(buf);
        vsnprintf(buf + len, 0xfff - len, fmt, ap);
        CLog* log = CLog::instance();
        Guard<Mutex> g((Mutex*)((char*)log + m_level * 0x18));
        switch (m_level) {
        case 0:
            CLog::instance()->logNotice(buf);
            break;
        case 1:
            CLog::instance()->logError(buf);
            break;
        case 2:
            CLog::instance()->logDebug(buf);
            break;
        case 3:
            CLog::instance()->logConsole(buf);
            break;
        case 4:
            CLog::instance()->logMoney(buf);
            break;
        case 5:
            CLog::instance()->logCritical(buf);
            break;
        }
    }
}
