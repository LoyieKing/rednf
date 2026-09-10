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
