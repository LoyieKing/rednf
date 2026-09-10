void NSLDBThread::PushTransaction(Message* pMessage)
{
    pthread_mutex_lock(&dbLock);
    mQueueSize = mQueueSize + 1;
    dbQueue.push(pMessage);
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&dbLock);
