Message* DataPool::getLogMessage(TCPUser* u)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LogSendMsgLock);
    Message* msg = LogSendMessagePool->malloc();
    msg->setUserToMessage(u);
    msg->setOnDataTypeMask(u->getSendDataType());
    SendBuffer* pSendBuffer = BufferLogSend->malloc();
    msg->setStringToMessage(pSendBuffer);
    return msg;
