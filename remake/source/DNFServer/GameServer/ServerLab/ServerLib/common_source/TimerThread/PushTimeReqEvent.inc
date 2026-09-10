void TimerThread::PushTimeReqEvent(ITimeEntity* pTEvent)
{
    pthread_mutex_lock(&timerLock);
    timeReqQueue.push(pTEvent);
    G_TraceLog()->sysLog(8, "TEVENT PUSH ID=%d", pTEvent->proc_id);
    pthread_mutex_unlock(&timerLock);
