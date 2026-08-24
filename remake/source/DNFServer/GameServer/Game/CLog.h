#ifndef DNF_GAME_CLOG_H_
#define DNF_GAME_CLOG_H_

class CLog
{
public:
    static CLog* instance();
    static void destroyInstance();  // ORIG 0x82a3cfd（W）
    static void createInstance();   // ORIG _ZN4CLog14createInstanceEv
    void initiate(char* name, char* path, int flag);  // ORIG _ZN4CLog8initiateEPcS0_i
    void logNotice(const char* fmt, ...);
    void logError(const char* fmt, ...);
    void logDebug(const char* fmt, ...);
    void logConsole(const char* fmt, ...);
    void logMoney(const char* fmt, ...);
    void logCritical(const char* fmt, ...);
    bool logSnapShot(const char* fmt, ...);            // ORIG 0x854f4c0
    bool logInit(bool bForceOpen, bool bAppend, const char* fmt, ...);  // ORIG 0x854f520
    bool logHistory(const char* fmt, char* vaArgs);   // ORIG 0x854f2e2（va_list 按栈传递）

    char m_mutexes[10 * 0x18];
    int m_logFlags[8];
    char m_initFile[0x100];
    char m_logFile[0x100];

private:
    CLog();
};

#endif
