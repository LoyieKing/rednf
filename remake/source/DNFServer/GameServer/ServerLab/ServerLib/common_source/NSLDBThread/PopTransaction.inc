Message* NSLDBThread::PopTransaction()
{
    pthread_mutex_lock(&dbLock);
    while (dbQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &dbLock);
    }
    Message* pMsg = dbQueue.front();
    dbQueue.pop();
    mQueueSize = mQueueSize - 1;
    pthread_mutex_unlock(&dbLock);
    return pMsg;
