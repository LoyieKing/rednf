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
    if (ent->bWillDelete == false)
    {
        G_TraceLog()->sysLog(8, "11Time Event Delete!!!!\n");
        super_TimeManager.delete2PeriodQueue(ent);
    }
    else
    {
        super_TimeManager.insert2PeriodQueue(ent);
    }
    timeReqQueue.pop();
    pthread_mutex_unlock(&timerLock);
    return ent;
}

void TimerThread::loop(void* temp)
{
    unsigned int elapsedTime = 0;
    do
    {
        while (true)
        {
            unsigned int startTime = (unsigned int)time(NULL);
            PopTimeReqEvent();
            super_TimeManager.onTime();
            unsigned int endTime = (unsigned int)time(NULL);
            elapsedTime = endTime - startTime;
            int carry = ((int)endTime >> 0x1f) - ((int)startTime >> 0x1f) - (unsigned int)(endTime < startTime);
            if (carry != 0 || 0x14 < elapsedTime)
            {
                break;
            }
            if ((int)(0x14 - elapsedTime) < 0)
            {
                TSystem<LinuxSystem>::sleep(0);
                printf("\xBD\xBD\xB8\xB3 \xC5\xB8\xC0\xD3 : %d\n", 0x14 - elapsedTime);
            }
            else
            {
                TSystem<LinuxSystem>::sleep(0x14 - elapsedTime);
            }
        }
        puts("TimerThread spent more time than NEXT_CHECK_TIME");
        printf("Elapsed Time: %lld\n", (long long)elapsedTime);
        unsigned int size = super_TimeManager.timePeriodMap.Size();
        printf("Map size: %d\n", size);
    } while (true);
}

} // namespace nsl
