#include "TimeManager.h"
#include "TimerThread.h"
#include "InternalMsg.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "WorkThread.h"
#include "Message.h"

namespace nsl {

TimeManager::TimeManager()
{
    last_tick = 0;
    cur_tick = 0;
    elapsedTick = 0;
    mtotal_entity = 0;
}

TimeManager::~TimeManager()
{
}

void TimeManager::insert2PeriodQueue(ITimeEntity* ent)
{
    if (!timePeriodMap.Insert(ent->entNo, ent))
    {
        G_TraceLog()->sysLog(8, "Fail: ==>Can't Insert TimeEntity!! uid=%d, pid=%d", ent->entNo, ent->proc_id);
        G_TraceLog()->sysLog(7, "Fail: ==>Can't Insert TimeEntity!! uid=%d, pid=%d", ent->entNo, ent->proc_id);
    }
    else
    {
        G_TraceLog()->sysLog(8, "Success: ==>Insert TimeEntity!! uid=%d, pid=%d", ent->entNo, ent->proc_id);
    }
}

void TimeManager::delete2PeriodQueue(ITimeEntity* ent)
{
    if (!timePeriodMap.Remove(ent->entNo))
    {
        G_TraceLog()->sysLog(7, "Fail: ==>Not found Time Entity entNo(%d), id(%d)", ent->entNo, ent->proc_id);
    }
    ent->setTerminated();
    InternalMsg* pArg = ent->getArg();
    pApp->super_Threads.getWorkThread(pArg->workIndex)->PushTransaction((IMessageStruct*)ent);
}

void TimeManager::onTime()
{
    cur_tick = pApp->getTick();
    elapsedTick = cur_tick - last_tick;
    iter = timePeriodMap.InorderBegin();
    while (iter.GetDataPtr() != NULL)
    {
        ITimeEntity* ent = *iter.GetDataPtr();
        ent->accumulated_tick += elapsedTick;
        if ((unsigned long long)ent->check_period <= (unsigned long long)ent->accumulated_tick)
        {
            InternalMsg* pArg = ent->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)->PushTransaction((IMessageStruct*)ent);
            ent->accumulated_tick = 0;
        }
        ++iter;
    }
    last_tick = cur_tick;
}

void TimeManager::preReservedMap()
{
}

} // namespace nsl
