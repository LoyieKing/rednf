#ifndef SYSTEM_TIME_HANDLER_H_
#define SYSTEM_TIME_HANDLER_H_

#include <sys/time.h>

// CommonTime：6 字节（y-100/m/d/h/m/w）
class CommonTime
{
public:
    void SetCurTime();
    char m_time[6];
};

// CSystemTime / CSystemTimeHandler（0x14）
class CSystemTime
{
public:
    CSystemTime()
    {
        gettimeofday(&m_tv, 0);
        m_sec = m_tv.tv_sec;
        m_msec = m_tv.tv_usec / 1000;
    }
    ~CSystemTime() {}
    int m_field0;     // +0
    int m_msec;       // +4
    struct timeval m_tv;  // +8
    int m_sec;        // +0x10
};

class CSystemTimeHandler : public CSystemTime
{
public:
    CSystemTimeHandler();
    ~CSystemTimeHandler() {}
};

CSystemTimeHandler* CSystemTimeHandlerInstance();

#endif // SYSTEM_TIME_HANDLER_H_
