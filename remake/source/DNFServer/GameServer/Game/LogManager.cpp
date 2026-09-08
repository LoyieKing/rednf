// df_game_r 还原 —— LogManager（G5 日志族）
// 参照 docs/class_func_reports/LogManager.md；ORIG 布局：
//   s_data   0x09501e60  static LogManagerData*（std::array<shared_ptr<LogWriter>,64>）
//   s_indent 0x09501e64  static int
// 全部 LogManager 方法为静态成员；prepareData 为文件内 static。
#include "LogManager.h"
#include <memory>
#define LOG_WRITER_SP std::shared_ptr

#include <array>
#include <cstdarg>
#include <cstdio>
#include <new>
#include <string>

struct LogManagerData
{
    std::array<LOG_WRITER_SP<LogWriter>, 64> data;
};

static LogManagerData* s_data = 0;
static int s_indent = 0;

static void prepareData()
{
    if (!s_data)
    {
        s_data = new LogManagerData();
    }
}

// ORIG 0x8ad40b8：_ZN23_LogManagerData_createrC1Ev / C2Ev（各 0xd 字节）
//   构造器仅调用 prepareData()；全局实例使 s_data 在静态初始化期完成分配
//   （ORIG 生命周期：启动即建，非懒初始化）。ORIG 无该类的 D1/D0 符号，
//   故无用户析构（trivial）。
class _LogManagerData_creater
{
public:
    _LogManagerData_creater() { prepareData(); }
};

static _LogManagerData_creater _LogManagerData_creater_instance;   // ORIG _ZL32_LogManagerData_creater_instance 0x09501e68

LogWriter::LogWriter()
{
}

LogWriter::~LogWriter()
{
}

void LogManager::logFormat(int level, const char* file, const char* func,
                           int line, const char* fmt, ...)
{
    // ORIG 权威二进制运行时行为 = no-op：入口字节 0x8ad3c0a 被热补为 0xc3(ret)
    // （原 push %ebp 0x55 覆盖，nm size 0x197 含 patch 前死代码体；Ghidra C 同判
    // `{ return; }`，objdump -s 实证）。重建为空体对齐 ORIG 可观察行为。
    (void)level; (void)file; (void)func; (void)line; (void)fmt;
}

void LogManager::logString(int level, const char* file, const char* func,
                           int line, const char* msg)
{
    prepareData();
    if (s_data->data[level])
    {
        if (s_indent > 0)
        {
            std::string str;
            for (int i = 0; i < s_indent; ++i)
            {
                str += "\t";
            }
            str += msg;
            s_data->data[level]->log(file, func, line, str.c_str());
        }
        else
        {
            s_data->data[level]->log(file, func, line, msg);
        }
    }
}

LOG_WRITER_SP<LogWriter> LogManager::getLogWriter(int index)
{
    prepareData();
    return s_data->data[index];
}

void LogManager::setLogWriter(int index, LOG_WRITER_SP<LogWriter> writer)
{
    prepareData();
    s_data->data[index] = writer;
}

void LogManager::pushIndent()
{
    ++s_indent;
}

void LogManager::popIndent()
{
    if (s_indent > 0)
    {
        --s_indent;
    }
}
