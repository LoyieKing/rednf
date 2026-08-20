#ifndef DNF_GAME_CLOG_H_
#define DNF_GAME_CLOG_H_

class CLog
{
public:
    static CLog* instance();
    void logNotice(const char* fmt, ...);
    void logError(const char* fmt, ...);
    void logDebug(const char* fmt, ...);
    void logConsole(const char* fmt, ...);
    void logMoney(const char* fmt, ...);
    void logCritical(const char* fmt, ...);

    char m_mutexes[10 * 0x18];
    int m_logFlags[8];
    char m_initFile[0x100];
    char m_logFile[0x100];

private:
    CLog();
};

#endif
