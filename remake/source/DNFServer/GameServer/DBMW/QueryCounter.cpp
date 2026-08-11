// df_dbmw_r - QueryCounter (ORIG QueryCounter.cpp)
#include "DBMWCommon.h"

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

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// CQueryCounter（dbmw 布局：+0 used[0x141] / +0x148 counts /
// +0x64c responseTimes / +0x1054 interval / +0x1058 timer）
CQueryCounter::CQueryCounter()
{
    m_interval = 0x1e;
    m_timer = new CUnixTimer;
    ResetQueryCount();
}
CQueryCounter::~CQueryCounter()
{
    operator delete(m_timer);
}
void CQueryCounter::ResetQueryCount()
{
    m_interval = 0x1e;
    memset((char*)this + 0x148, 0, 0x504);
    memset((char*)this + 0x64c, 0, 0xa08);
    memset((char*)this + 0x4, 0, 0x141);
    m_field0 = 0;
}
void CQueryCounter::WriteDBLog(CDBManager& db)
{
    m_interval--;
    if (m_interval > 0)
        return;
    for (int q = 0x4e21; q <= 0x4f60; q++)
    {
        int idx = q - 0x4e20;
        int time = (int)(m_responseTimes[idx] * 1000.0);
        if (!db.UpdateQueryCount(q, m_counts[idx], time))
        {
            if (m_counts[idx] != 0)
            {
                CMyFileLog log(__FUNCTION__, 0x76);
                log("./log/QueryCount",
                    "Count DB Insert Fail! id(%d), count(%d), time(%d)", q,
                    m_counts[idx], time);
            }
        }
        else
        {
            if (m_counts[idx] != 0)
            {
                int avg = time / m_counts[idx];
                CMyFileLog log(__FUNCTION__, 0x7a);
                log("./log/QueryCount",
                    "Count DB Insert Success! id(%d), count(%d), time(%d), compute(%4.2f)",
                    q, m_counts[idx], time, avg);
                m_counts[idx] = 0;
                m_responseTimes[idx] = 0.0;
            }
        }
    }
    m_interval = 0x1e;
}
char CQueryCounter::IncreQureyCount(unsigned int idx, const char* name)
{
    if (idx <= 0x4f60)
    {
        int i = idx - 0x4e20;
        m_counts[i]++;
        m_timer->SetLastTime();
        return !m_used[i];
    }
}
void CQueryCounter::SetResponseTime(unsigned int ms)
{
    if (ms > 0x4f60)
        return;
    int i = ms - 0x4e20;
    double interval = m_timer->GetTimeInterval();
    m_responseTimes[i] = interval + m_responseTimes[i];
    if (interval > 500.0)
    {
        CMyFileLog log(__FUNCTION__, 0x5d);
        log("./log/SlowQuery", "type(%d)interval(%d)", ms, interval);
    }
}
int CQueryCounter::LoadQueryIdTable(int queryId)
{
    if (queryId > 0x4e20 && queryId <= 0x4f60)
    {
        int i = queryId - 0x4e20;
        m_used[i] = 1;
        return 1;
    }
    return 0;
}

// force rebuild

// y

// z
