#ifndef DBMW_SYSTEMTIMEHANDLER_H_
#define DBMW_SYSTEMTIMEHANDLER_H_

#include "DBMWCommon.h"


class CSystemTime
{
public:
    CSystemTime();
    ~CSystemTime() {}
    int m_field0;       // +0
    int m_field4;       // +4
    struct timeval m_tv;  // +8
    int m_field10;      // +0x10
};

class CSystemTimeHandler : public CSystemTime
{
public:
    CSystemTimeHandler() {}
    ~CSystemTimeHandler() {}
};

class CDnFTimer
{
public:
    CDnFTimer() {}  // ORIG inline（弱符号，QueryCounter.cpp 使用处发出，sub $0x18 帧）
    ~CDnFTimer() {}
    virtual void SetLastTime() {}
    virtual double GetTimeInterval() { return 0.0; }
};

class CUnixTimer : public CDnFTimer
{
public:
    CUnixTimer() throw() {}  // ORIG inline + 空异常规格：CQueryCounterC1 的 new 不产生 EH 清理块
    virtual void SetLastTime();
    virtual double GetTimeInterval();
    double GetNowTime();  // 非虚（原版无虚表槽）
    double m_lastTime;  // +4
};

class CommonTime
{
public:
    void SetCurTime();
    char m_year;   // +0
    char m_mon;    // +1
    char m_mday;   // +2
    char m_hour;   // +3
    char m_min;    // +4
    char m_sec;    // +5
};

#endif  // DBMW_SYSTEMTIMEHANDLER_H_
