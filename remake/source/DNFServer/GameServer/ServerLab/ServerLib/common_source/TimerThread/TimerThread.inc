TimerThread::TimerThread()
    : timeReqQueue()
{
    pthread_mutex_init(&timerLock, NULL);
