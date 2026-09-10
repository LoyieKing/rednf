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
