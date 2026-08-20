#include "CLog.h"

CLog::CLog()
{
}

CLog* CLog::instance()
{
    static CLog log;
    return &log;
}

#define CLOG_NOOP(name) \
    void CLog::name(const char*, ...) {}

CLOG_NOOP(logNotice)
CLOG_NOOP(logError)
CLOG_NOOP(logDebug)
CLOG_NOOP(logConsole)
CLOG_NOOP(logMoney)
CLOG_NOOP(logCritical)
