IMessageStruct* WorkThread::PopTransaction()
{
    pthread_mutex_lock(&workerLock);
    while (orderQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &workerLock);
    }
    Message* msg = (Message*)orderQueue.front();
    orderQueue.pop();
    // ORIG: jbe join(nop); Dec; jmp join+1; nop; join: mQueueSize--  (goto 复现空块/nop)
    if ((unsigned int)((int)msg->mMsgType - 1) <= 2)
    {
        goto join;
    }
    msg->getUserFromMessage()->DecPendingWorkNum();
join:
    mQueueSize = mQueueSize - 1;
    pthread_mutex_unlock(&workerLock);
    return (IMessageStruct*)msg;
