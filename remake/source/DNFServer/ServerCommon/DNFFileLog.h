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
// 2026-08-11 二进制实测：ORIG 展开为临时对象直接调用 operator()——
// `CMyFileLog log(...); log(args)` 的两语句形式会让 operator() 的实参（如
// *(ushort*)pkt、e.what()）在 ctor 之后求值，与 ORIG 逐条不符；临时对象形式
// 下实参在临时构造前按从右到左求值并装入 callee-saved 寄存器（ebx/esi/edi），
// 与 ORIG MsgDecode/CNetworkThread 等反汇编逐字节一致（t4 验证）。
#define DNF_LOG_SCOPE_LINE(line, ...) CMyFileLog(__FUNCTION__, line)(__VA_ARGS__)

// 全手动版本：函数名与行号都显式传入（用于名字不是函数短名的少数调用点）。
// ORIG 实测（coserver UdpHandler）：单表达式临时对象形式 —— 带数据实参的调用
// 在构造前先求值实参（进入 callee-saved 寄存器）；两句式会在构造后求值。
// 语义等价，仅影响代码生成形态。
#define DNF_LOG_SCOPE_AT(name, line, ...) CMyFileLog(name, line)(__VA_ARGS__)

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

// 无换行变体：原版部分 handler 的 Exception Break 文案不带 \n。
#define DNF_CATCH_LOG_NN(logfile, msg, line_e, line_all) \
    catch (CDNFException& e) \
    { \
        DNF_LOG_SCOPE_LINE(line_e, logfile, msg " : %s", e.what()); \
    } \
    catch (...) \
    { \
        DNF_LOG_SCOPE_LINE(line_all, logfile, msg); \
    }

// 双消息变体：原版部分 handler 的 CDNFException 消息缺 "()"（如
// "OnQueryFirstLoadSpecDb Exception Break"），catch-all 却带 "()"。
#define DNF_CATCH_LOG2(logfile, msg1, msg2, line_e, line_all) \
    catch (CDNFException& e) \
    { \
        DNF_LOG_SCOPE_LINE(line_e, logfile, msg1 " : %s\n", e.what()); \
    } \
    catch (...) \
    { \
        DNF_LOG_SCOPE_LINE(line_all, logfile, msg2 "\n"); \
    }

// printf + log 双写家族：CDNFException 先 printf 再写 Except 日志。
#define DNF_CATCH_LOG_PRINTF(logfile, msg, line_e, line_all) \
    catch (CDNFException& e) \
    { \
        printf(msg " : %s\n", e.what()); \
        DNF_LOG_SCOPE_LINE(line_e, logfile, msg " : %s\n", e.what()); \
    } \
    catch (...) \
    { \
        puts(msg); \
        DNF_LOG_SCOPE_LINE(line_all, logfile, msg "\n"); \
    }

// 单 catch 变体：ORIG 部分 handler 只有 CDNFException 分支且文案无换行。
#define DNF_CATCH_LOG_CDNF(logfile, msg, line_e) \
    catch (CDNFException& e) \
    { \
        DNF_LOG_SCOPE_LINE(line_e, logfile, msg " : %s", e.what()); \
    }

// 双日志文件 + 双消息变体：catch1 无换行、catch2 有换行，路径各自不同。
#define DNF_CATCH_LOG2_NN(logfile_e, logfile_all, msg1, msg2, line_e, line_all) \
    catch (CDNFException& e) \
    { \
        DNF_LOG_SCOPE_LINE(line_e, logfile_e, msg1 " : %s", e.what()); \
    } \
    catch (...) \
    { \
        DNF_LOG_SCOPE_LINE(line_all, logfile_all, msg2 "\n"); \
    }

// catch 带 return 0 变体（如 OnRequestIPCounterList 的 char 返回值路径）。
#define DNF_CATCH_LOG_RET0(logfile, msg, line_e, line_all) \
    catch (CDNFException& e) \
    { \
        DNF_LOG_SCOPE_LINE(line_e, logfile, msg " : %s\n", e.what()); \
        return 0; \
    } \
    catch (...) \
    { \
        DNF_LOG_SCOPE_LINE(line_all, logfile, msg "\n"); \
        return 0; \
    }

// rethrow 变体：CDNFException 先 printf 再 throw，catch-all 仅 puts 后 throw。
#define DNF_CATCH_LOG_THROW(msg) \
    catch (CDNFException& e) \
    { \
        printf(msg " : %s\n", e.what()); \
        throw; \
    } \
    catch (...) \
    { \
        puts(msg "\n"); \
        throw; \
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
