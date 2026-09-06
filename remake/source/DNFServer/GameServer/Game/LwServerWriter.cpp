// df_game_r 还原 —— LwServerWriter（CLog 构造注册的日志 writer）。
// 报告：docs/class_func_reports/LwServerWriter/{LwServerWriter,log}.md
#include "LwServerWriter.h"
#include "CLog.h"

#include <cstdio>

LwServerWriter::LwServerWriter(ENUM_LOG_TYPE type)
    : m_type(type)
{
}

LwServerWriter::~LwServerWriter()
{
}

void LwServerWriter::log(char const* /*file*/, char const* func, int line,
                         char const* msg)
{
    char buf[0x1000];
    if (func != 0)
        snprintf(buf, 0xfff, "%s(%d): %s", func, line, msg);
    else
        snprintf(buf, 0xfff, "%s", msg);

    Guard<Mutex> guard(&CLog::instance()->m_mutexes[m_type]);
    switch (m_type) {
    case ENUM_LOG_TYPE_1:
        CLog::instance()->logError(buf);
        break;
    case ENUM_LOG_TYPE_2:
        CLog::instance()->logDebug(buf);
        break;
    }
}
