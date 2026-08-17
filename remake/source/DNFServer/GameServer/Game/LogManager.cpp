// df_game_r 还原 —— LogManager（G5 日志族）
// 参照 docs/class_func_reports/LogManager.md；ORIG 布局：
//   s_data   0x09501e60  static LogManagerData*（std::array<shared_ptr<LogWriter>,64>）
//   s_indent 0x09501e64  static int
// 全部 LogManager 方法为静态成员；prepareData 为文件内 static。
#include "LogManager.h"

#include <array>
#include <cstdarg>
#include <cstdio>
#include <new>
#include <string>

struct LogManagerData
{
    std::array<std::shared_ptr<LogWriter>, 64> data;
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

LogWriter::LogWriter()
{
}

LogWriter::~LogWriter()
{
}

void LogManager::logFormat(int level, const char* file, const char* func,
                           int line, const char* fmt, ...)
{
    prepareData();
    if (s_data->data[level])
    {
        char buf[0x4000];
        va_list ap;
        va_start(ap, fmt);
        if (vsnprintf(buf, sizeof(buf), fmt, ap) == -1)
        {
            buf[sizeof(buf) - 1] = 0;
        }
        va_end(ap);
        if (s_indent > 0)
        {
            std::string str;
            for (int i = 0; i < s_indent; ++i)
            {
                str += "\t";
            }
            str += buf;
            s_data->data[level]->writeLog(file, func, line, str.c_str());
        }
        else
        {
            s_data->data[level]->writeLog(file, func, line, buf);
        }
    }
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
            s_data->data[level]->writeLog(file, func, line, str.c_str());
        }
        else
        {
            s_data->data[level]->writeLog(file, func, line, msg);
        }
    }
}

std::shared_ptr<LogWriter> LogManager::getLogWriter(int index)
{
    prepareData();
    return s_data->data[index];
}

void LogManager::setLogWriter(int index, std::shared_ptr<LogWriter> writer)
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
