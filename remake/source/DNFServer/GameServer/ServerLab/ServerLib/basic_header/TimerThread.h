#ifndef NSL_TIMERTHREAD_H_
#define NSL_TIMERTHREAD_H_

#include <pthread.h>
#include <queue>

#include "Thread.h"
#include "TimeManager.h"
#include "Message.h"

namespace nsl {

class Message;
class InternalMsg;

typedef unsigned long long __int64;

class ITimeEntity : public IMessageStruct
{
public:
    ITimeEntity()
    {
        bTerminated = false;
    }
    virtual ~ITimeEntity()
    {
    }
    virtual int operator()() = 0;
    inline void setTerminated()
    {
        bTerminated = true;
    }
    inline bool isTerminated()
    {
        return bTerminated;
    }
    inline void setArg(InternalMsg* pMsg)
    {
        pmMsg = pMsg;
    }
    inline InternalMsg* getArg()
    {
        return pmMsg;
    }

    bool bTerminated;       // tail padding of IMessageStruct @7
    int proc_id;
    unsigned int check_period;
    unsigned int proc_count;
    bool bWillDelete;
    bool bRunning;
    __int64 accumulated_tick;
    unsigned int entNo;
    InternalMsg* pmMsg;
};

// ORIG DWARF/反汇编（point/auction 同源）：TimeManager 为第二基类
// （偏移 0x14），非成员——ctor 中 vtable 存储在两个基类 ctor 之后、
// timeReqQueue 成员 ctor 之前；写成成员会使 GCC 在 Thread 基类 ctor 后
// 立即写 vtable（TimerThreadC1 差异根因）。布局不变（仍位于 0x14）。
class TimerThread : public Thread, public TimeManager
{
public:
    TimerThread();
    virtual ~TimerThread();
    virtual void loop(void* temp);
    void PushTimeReqEvent(ITimeEntity* pTimeEntity);
    ITimeEntity* PopTimeReqEvent();

    std::queue<ITimeEntity*> timeReqQueue;
    pthread_mutex_t timerLock;
};

} // namespace nsl

#endif // NSL_TIMERTHREAD_H_
