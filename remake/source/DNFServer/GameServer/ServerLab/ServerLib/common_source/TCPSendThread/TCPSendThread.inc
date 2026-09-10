TCPSendThread::TCPSendThread()
    : sendQueue()
{
    mQueueSize = 0;
    sequence = 0;
    pthread_mutex_init(&senderLock, NULL);
    pthread_cond_init(&isEmpty, NULL);
