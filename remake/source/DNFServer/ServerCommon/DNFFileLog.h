#ifndef REDNF_DNFFILELOG_H_
#define REDNF_DNFFILELOG_H_

#include <cstdio>
#include <ctime>
#include <map>
#include <string>

#include "Thread.h"

// 日志类，布局/行为来自 Ghidra 反编译（2026-08-06）
class CFLog {
public:
    CFLog(std::string& filename);
    virtual ~CFLog();
    // 原始：writeLog 均为非虚（vtable 仅 D1/D0 两个槽，反汇编确认调用为直接 call）
    void writeLog(tm* t, char const* fmt, char* va);
    void writeLog(char const* fmt, char* va);

    FILE* file;          // offset 4
    std::string name;    // offset 8
};

class CFileLogWriter {
public:
    CFileLogWriter();
    virtual ~CFileLogWriter();
    void writeLog(char const* file, char const* fmt, ...);
    void writeRawLog(char const* file, char const* fmt, ...);

    std::map<std::string, CFLog*> logs;  // offset 4
    CMutex mutex;                         // offset 0x1c
};

class CMyFileLog {
public:
    CMyFileLog(char const* name, int level);
    void operator()(char const* file, char const* fmt, ...);

    char const* name;  // offset 0
    int level;         // offset 4
};

class CMyRawFileLog {
public:
    void operator()(char const* file, char const* fmt, ...);
};

// 原始源码中 CMyFileLog 由宏生成：名字取自函数名、级别取自调用处行号（__LINE__）。
// 重构版本把两者硬编码成了字面量；恢复宏后需保证调用处实际行号与硬编码值一致
// （见 docs/line_number_alignment_report.md）。
// 三个变体都同时包含紧随其后的 log(...) 调用：展开为
//   CMyFileLog log(<name>, <line>); log(<args>);
#define DNF_LOG_SCOPE(...) CMyFileLog log(__FUNCTION__, __LINE__); log(__VA_ARGS__)

// 显式行号版本：函数名自动（__FUNCTION__），行号由调用处显式传入（重构保真的 0xNN）。
// 适用于尚未做行号对齐的文件——值不变，仅把构造抽成宏。
#define DNF_LOG_SCOPE_LINE(line, ...) CMyFileLog log(__FUNCTION__, line); log(__VA_ARGS__)

// 全手动版本：函数名与行号都显式传入（用于名字不是函数短名的少数调用点）。
#define DNF_LOG_SCOPE_AT(name, line, ...) CMyFileLog log(name, line); log(__VA_ARGS__)

// 通用 try/catch 冗余：log 家族（CDNFException + catch-all 都写 Except 日志）。
// msg 为共享消息前缀（如 "Xxx() Exception Break"），展开与手写逐 token 相同。
#define DNF_CATCH_LOG(logfile, msg, line_e, line_all) \
    catch (CDNFException& e) \
    { \
        DNF_LOG_SCOPE_LINE(line_e, logfile, msg " : %s\n", e.what()); \
    } \
    catch (...) \
    { \
        DNF_LOG_SCOPE_LINE(line_all, logfile, msg "\n"); \
    }

// 通用 try/catch 冗余：printf + 原样重抛家族。
#define DNF_CATCH_RETHROW(msg) \
    catch (CDNFException& e) \
    { \
        printf(msg " : %s\n", e.what()); \
        throw; \
    } \
    catch (...) \
    { \
        puts(msg); \
        throw; \
    }

class CToolFileLog {
public:
    CToolFileLog(char const* name, int level);
    void operator()(char const* file, int seq, char const* fmt, ...);

    char const* name;  // offset 0
    int level;         // offset 4
};

CFileLogWriter* CFileLogWriterInstance();

#endif  // REDNF_DNFFILELOG_H_
