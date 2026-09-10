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
        insert2PeriodQueue(ent);
    }
    else
    {
        G_TraceLog()->sysLog(8, "11Time Event Delete!!!!\n");
        delete2PeriodQueue(ent);
    }
    timeReqQueue.pop();
    pthread_mutex_unlock(&timerLock);
    return ent;
