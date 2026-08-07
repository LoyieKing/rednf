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

class CToolFileLog {
public:
    CToolFileLog(char const* name, int level);
    void operator()(char const* file, int seq, char const* fmt, ...);

    char const* name;  // offset 0
    int level;         // offset 4
};

CFileLogWriter* CFileLogWriterInstance();

#endif  // REDNF_DNFFILELOG_H_
