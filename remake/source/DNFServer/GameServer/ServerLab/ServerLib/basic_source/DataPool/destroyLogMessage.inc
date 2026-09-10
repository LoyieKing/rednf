void DataPool::destroyLogMessage(Message* msg)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LogSendMsgLock);
    BufferLogSend->free(msg->getSendBufferFromMessage());
    LogSendMessagePool->free(msg);
