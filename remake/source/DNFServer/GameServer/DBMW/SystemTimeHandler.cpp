// df_dbmw_r - SystemTimeHandler (ORIG SystemTimeHandler.cpp)
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DBMWCommon.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// ============================================================
// CSystemTime / CSystemTimeHandler
// ============================================================
CSystemTime::CSystemTime()
{
    gettimeofday(&m_tv, 0);
    m_field10 = m_tv.tv_sec;
    m_field4 = m_tv.tv_usec / 1000;
}
CSystemTimeHandler* CSystemTimeHandlerInstance()
{
    static CSystemTimeHandler instance;
    return &instance;
}
double CUnixTimer::GetNowTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
double CUnixTimer::GetTimeInterval()
{
    return GetNowTime() - m_lastTime;
}
void CUnixTimer::SetLastTime()
{
    m_lastTime = GetNowTime();
}
void CommonTime::SetCurTime()
{
    time_t t;
    time(&t);
    struct tm* tm = localtime(&t);
    m_year = tm->tm_year - 0x64;
    m_mon = tm->tm_mon + 1;
    m_mday = tm->tm_mday;
    m_hour = tm->tm_hour;
    m_min = tm->tm_min;
    m_sec = tm->tm_wday;
}
