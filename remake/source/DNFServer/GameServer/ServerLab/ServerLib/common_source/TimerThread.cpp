#include <stdio.h>
#include <time.h>

#include "TimerThread.h"
#include "TraceLog.h"
#include "System.h"

namespace nsl {

TimerThread::TimerThread()
    : timeReqQueue()
{
    pthread_mutex_init(&timerLock, NULL);
}

TimerThread::~TimerThread()
{
}

void TimerThread::PushTimeReqEvent(ITimeEntity* pTEvent)
{
    pthread_mutex_lock(&timerLock);
    timeReqQueue.push(pTEvent);
    G_TraceLog()->sysLog(8, "TEVENT PUSH ID=%d", pTEvent->proc_id);
    pthread_mutex_unlock(&timerLock);
}

ITimeEntity* TimerThread::PopTimeReqEvent()
{
    pthread_mutex_lock(&timerLock);
    if (timeReqQueue.size() == 0)
    {
        pthread_mutex_unlock(&timerLock);
        return NULL;
    }
    ITimeEntity* ent = timeReqQueue.front();
    // ORIG: if (bWillDelete) insert; else log+delete  (test/je delete path)
    if (ent->bWillDelete)
    {
        super_TimeManager.insert2PeriodQueue(ent);
    }
    else
    {
        G_TraceLog()->sysLog(8, "11Time Event Delete!!!!\n");
        super_TimeManager.delete2PeriodQueue(ent);
    }
    timeReqQueue.pop();
    pthread_mutex_unlock(&timerLock);
    return ent;
}

void TimerThread::loop(void* temp)
{
    // ORIG: sign-extend time() to 64-bit via sar, sub/sbb into locals, cmpl mem for unsigned >
    do
    {
        long long startTime = (long long)time(NULL);
        PopTimeReqEvent();
        super_TimeManager.onTime();
        long long endTime = (long long)time(NULL);
        unsigned long long elapsedTime = (unsigned long long)endTime - (unsigned long long)startTime;
        if (elapsedTime > (unsigned long long)0x14)
        {
            puts("TimerThread spent more time than NEXT_CHECK_TIME");
            printf("Elapsed Time: %lld\n", (long long)elapsedTime);
            printf("Map size: %d\n", super_TimeManager.timePeriodMap.Size());
        }
        else
        {
            if (0x14 - (int)elapsedTime < 0)
            {
                TSystem<LinuxSystem>::sleep(0);
                printf("\xBD\xBD\xB8\xB3 \xC5\xB8\xC0\xD3 : %d\n", 0x14 - (int)elapsedTime);
            }
            else
            {
                TSystem<LinuxSystem>::sleep(0x14 - (int)elapsedTime);
            }
        }
    } while (true);
}

} // namespace nsl
