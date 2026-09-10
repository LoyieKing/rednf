Message* TCPSendThread::PopSendMsg()
{
    G_TraceLog()->sysLog(8, "In PopSendUser this=%x", this);
    pthread_mutex_lock(&senderLock);
    while (sendQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &senderLock);
    }
    TCPUser* u = sendQueue.front();
    Message* msg = u->PopSendMessage();
    sendQueue.pop();
    mQueueSize = mQueueSize - 1;
    // ORIG: getUserFromMessage() result goes straight into DecPendingSendNum (no re-store to u)
    msg->getUserFromMessage()->DecPendingSendNum();
    pthread_mutex_unlock(&senderLock);
    G_TraceLog()->sysLog(8, "Out PopSendUser");
    return msg;
