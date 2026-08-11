#ifndef QUERY_COUNTER_H_
#define QUERY_COUNTER_H_

class CDBManager;
class CUnixTimer;
class CDnFTimer;

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
    int m_counts[0x141];  // +0（0x504 字节，ORIG：int 数组直接入 edx 形态）
    double m_responseTimes[0x141]; // +0x504（0xa08 字节）
    int m_interval;  // +0xf0c
    CDnFTimer* m_timer;  // +0xf10（ORIG：基类指针赋值产生 mov ebx,eax;mov eax,edx 形态）
};

CQueryCounter* CQueryCounterInstance();

#endif  // QUERY_COUNTER_H_
