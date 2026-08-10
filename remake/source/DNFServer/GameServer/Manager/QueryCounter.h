#ifndef QUERY_COUNTER_H_
#define QUERY_COUNTER_H_

class CDBManager;
class CUnixTimer;

// ---- CQueryCounter ----
class CQueryCounter
{
public:
    CQueryCounter();
    ~CQueryCounter();
    void IncreQureyCount(unsigned int idx);
    void ResetQueryCount();
    void SetResponseTime(unsigned int ms);
    void WriteFileLog();
    void WriteDBLog(CDBManager& db);
    unsigned int m_counts[0x141];  // +0（0x504 字节）
    double m_responseTimes[0x141]; // +0x504（0xa08 字节）
    int m_interval;  // +0xf0c
    CUnixTimer* m_timer;  // +0xf10
};

CQueryCounter* CQueryCounterInstance();

#endif  // QUERY_COUNTER_H_
