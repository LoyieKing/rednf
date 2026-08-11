#ifndef SYSTEM_TIME_HANDLER_H_
#define SYSTEM_TIME_HANDLER_H_

#include <sys/time.h>

// ---- CSystemTime：0x14 ----
class CSystemTime
{
public:
    CSystemTime();
    ~CSystemTime() {}
    int m_field0;       // +0（ORIG 布局：+0 未初始化 int，m_tv 在 +8）
    unsigned int m_field4;  // +4（ORIG 为无符号：除 1000 后隐式转换产生 eax 物化形态，
                            //    与 coserver 修复一致，SystemTimeHandler.cpp 已核对）
    struct timeval m_tv;  // +8
    int m_field10;      // +0x10
};

class CSystemTimeHandler : public CSystemTime
{
public:
    CSystemTimeHandler() {}
    ~CSystemTimeHandler() {}
};

CSystemTimeHandler* CSystemTimeHandlerInstance();

// ---- CDnFTimer：vtable@0（SetLastTime / GetTimeInterval 两个虚函数）----
class CDnFTimer
{
public:
    CDnFTimer() {}  // ORIG inline（弱符号，QueryCounter.cpp 使用处发出，sub $0x18 帧）
    ~CDnFTimer() {}
    virtual void SetLastTime() {}
    virtual double GetTimeInterval() { return 0.0; }
};

// ---- CUnixTimer：+4 double ----
class CUnixTimer : public CDnFTimer
{
public:
    CUnixTimer() throw() {}  // ORIG inline + 空异常规格：CQueryCounterC1 的 new 不产生 EH 清理块
    ~CUnixTimer() {}
    virtual void SetLastTime();
    virtual double GetTimeInterval();
    double GetNowTime();  // 非虚（原版无虚表槽）
    double m_lastTime;  // +4
};

// ---- CommonTime ----
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

#endif  // SYSTEM_TIME_HANDLER_H_
