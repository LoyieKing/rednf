void TimeManager::delete2PeriodQueue(ITimeEntity* ent)
{
    if (!timePeriodMap.Remove(ent->entNo))
    {
        G_TraceLog()->sysLog(7, "Fail: ==>Not found Time Entity entNo(%d), id(%d)", ent->entNo, ent->proc_id);
    }
    ent->setTerminated();
    InternalMsg* pArg = ent->getArg();
    pApp->super_Threads.getWorkThread(pArg->workIndex)->PushTransaction((IMessageStruct*)ent);
