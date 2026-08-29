#ifndef DNF_GAME_LOGMANAGER_H_
#define DNF_GAME_LOGMANAGER_H_

// ============================================================================
// df_game_r 还原 —— LogManager（G5 日志族）+ LogWriter 最小声明
// 参照 docs/class_func_reports/LogManager.md / LogWriter.md。
// 布局：LogManager 全静态成员；LogWriter 纯虚 log 位于 vtable+8
// （ORIG vtable[2] = __cxa_pure_virtual；LwCallback/LwConsole/LwServerWriter
// 的 vtable+8 覆盖符号均为 `log`，LogWriter 自身无 writeLog 符号——
// 原 writeLog 命名与 ORIG 矛盾，2026-08-28 修正为 log）。
// ============================================================================

#include <memory>

class LogWriter
{
public:
    LogWriter();                    // 0x08ad3990
    virtual ~LogWriter();           // 0x08ad399e / D0 0x08ad39c4
    virtual void log(const char* file, const char* func,
                     int line, const char* msg) = 0;   // vtable+8
};

class LogManager
{
public:
    static void logFormat(int level, const char* file, const char* func,
                          int line, const char* fmt, ...);      // 0x08ad3c0a
    static void logString(int level, const char* file, const char* func,
                          int line, const char* msg);           // 0x08ad3da2
// 统一 std::shared_ptr（ORIG mangled `St10shared_ptr`；工具链 -std=gnu++0x 提供 <memory>）
    static std::shared_ptr<LogWriter> getLogWriter(int index);              // 0x08ad3f24
    static void setLogWriter(int index, std::shared_ptr<LogWriter> writer); // 0x08ad3f64
    static void pushIndent();       // 0x08ad3ef6
    static void popIndent();        // 0x08ad3f08
};

#endif  // DNF_GAME_LOGMANAGER_H_
