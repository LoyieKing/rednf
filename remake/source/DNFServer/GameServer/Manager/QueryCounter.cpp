// df_manager_r — CQueryCounter（ORIG QueryCounter.cpp）
#include "QueryCounter.h"

#include <stdio.h>
#include <string.h>

#include "DBManager.h"
#include "DNFFileLog.h"
#include "SystemTimeHandler.h"

CQueryCounter::CQueryCounter()
{
    m_interval = 0x1e;
    m_timer = new CUnixTimer;
    ResetQueryCount();
}

CQueryCounter::~CQueryCounter()
{
    ::operator delete(m_timer);
}

void CQueryCounter::ResetQueryCount()
{
    m_interval = 0x1e;
    memset(&m_counts, 0, 0x504);
    memset((char*)&m_counts + 0x504, 0, 0xa08);
}

void CQueryCounter::WriteFileLog()
{
    char buf[0x400] = {0};
    for (int i = 1; i <= 0x140; i++)
        sprintf(buf, "%s\t%d(%d)", buf, i, m_counts[i]);
    CMyFileLog log("WriteFileLog", 0x56);
    log("./log/QueryCount", "%s", buf);
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
            CMyFileLog log("WriteDBLog", 0x63);
            log("./log/QueryCount", "Count DB Insert Fail! id(%d), count(%d), time(%d)", q, m_counts[idx], time);
        }
        else
        {
            CMyFileLog log("WriteDBLog", 0x66);
            log("./log/QueryCount", "Count DB Insert Success! id(%d), count(%d), time(%d)", q, m_counts[idx], time);
            m_counts[idx] = 0;
            m_responseTimes[idx] = 0.0;
        }
    }
    m_interval = 0x1e;
}

void CQueryCounter::IncreQureyCount(unsigned int idx)
{
    if (idx > 0x4f60)
        return;
    int i = idx - 0x4e20;
    m_counts[i]++;
    m_timer->SetLastTime();
    CMyFileLog log("IncreQureyCount", 0x42);
    log("./log/QueryCount", "IncreQureyCount() type(%d) , Count(%d)!", i, m_counts[i]);
}

void CQueryCounter::SetResponseTime(unsigned int ms)
{
    if (ms > 0x4f60)
        return;
    double cur;
    int i = ms - 0x4e20;
    cur = m_responseTimes[i];
    m_responseTimes[i] = cur + m_timer->GetTimeInterval();
}

CQueryCounter* CQueryCounterInstance()
{
    static CQueryCounter instance;
    return &instance;
}
