NSLDBThread::NSLDBThread()
    : dbQueue()
{
    mQueueSize = 0;
    mTransactionCntPerSec = 0;
    pthread_mutex_init(&dbLock, NULL);
    pthread_cond_init(&isFull, NULL);
    pthread_cond_init(&isEmpty, NULL);
