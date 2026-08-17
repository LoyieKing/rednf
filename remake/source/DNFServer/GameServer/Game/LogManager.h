#ifndef DNF_GAME_LOGMANAGER_H_
#define DNF_GAME_LOGMANAGER_H_

// ============================================================================
// df_game_r 还原 —— LogManager（G5 日志族）+ LogWriter 最小声明
// 参照 docs/class_func_reports/LogManager.md / LogWriter.md。
// 布局：LogManager 全静态成员；LogWriter 纯虚 writeLog 位于 vtable+8
// （ORIG vtable[2] = __cxa_pure_virtual）。
// ============================================================================

#include <memory>

class LogWriter
{
public:
    LogWriter();                    // 0x08ad3990
    virtual ~LogWriter();           // 0x08ad399e / D0 0x08ad39c4
    virtual void writeLog(const char* file, const char* func,
                          int line, const char* msg) = 0;   // vtable+8
};

class LogManager
{
public:
    static void logFormat(int level, const char* file, const char* func,
                          int line, const char* fmt, ...);      // 0x08ad3c0a
    static void logString(int level, const char* file, const char* func,
                          int line, const char* msg);           // 0x08ad3da2
    static std::shared_ptr<LogWriter> getLogWriter(int index);  // 0x08ad3f24
    static void setLogWriter(int index, std::shared_ptr<LogWriter> writer); // 0x08ad3f64
    static void pushIndent();       // 0x08ad3ef6
    static void popIndent();        // 0x08ad3f08
};

#endif  // DNF_GAME_LOGMANAGER_H_
