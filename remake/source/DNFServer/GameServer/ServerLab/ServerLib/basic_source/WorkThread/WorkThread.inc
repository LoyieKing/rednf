WorkThread::WorkThread(int id)
    : orderQueue()
{
    OrderPool = new boost::object_pool<Message, boost::default_user_allocator_new_delete>(0x800, 0);
    mId = id;
    pthread_mutex_init(&workerLock, NULL);
    pthread_cond_init(&isEmpty, NULL);
    mQueueSize = 0;
    mTransactionCntPerSec = 0;
