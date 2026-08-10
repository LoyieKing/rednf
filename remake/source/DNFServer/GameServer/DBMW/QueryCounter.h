#ifndef DBMW_QUERYCOUNTER_H_
#define DBMW_QUERYCOUNTER_H_

#include "DBMWCommon.h"


class CQueryCounter
{
public:
    CQueryCounter();
    ~CQueryCounter();
    char IncreQureyCount(unsigned int idx, const char* name);
    void ResetQueryCount();
    void SetResponseTime(unsigned int ms);
    void WriteFileLog();
    void WriteDBLog(CDBManager& db);
    int LoadQueryIdTable(int queryId);
    int m_field0;                     // +0
    bool m_used[0x141];               // +4
    unsigned int m_counts[0x141];     // +0x148（0x504 字节）
    double m_responseTimes[0x141];    // +0x64c（0xa08 字节）
    int m_interval;                   // +0x1054
    CUnixTimer* m_timer;              // +0x1058
};

#endif  // DBMW_QUERYCOUNTER_H_
