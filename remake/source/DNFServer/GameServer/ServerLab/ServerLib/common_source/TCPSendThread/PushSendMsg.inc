void TCPSendThread::PushSendMsg(Message* msg)
{
    G_TraceLog()->sysLog(8, "In PushSendUser");
    pthread_mutex_lock(&senderLock);
    TCPUser* u = msg->getUserFromMessage();
    if ((u->isAboutToDisconnect()) || (u->isDisconnected()))
    {
        u->SetSending(false);
        msg->initialize();
        SendDataPoolFree(msg, u);
        // Pass __int64 directly — ORIG loads both 32-bit halves into ebx/esi
        G_TraceLog()->sysLog(8, "SEND FREE msg=%d", Message::ident);
        pthread_cond_signal(&isEmpty);
        pthread_mutex_unlock(&senderLock);
        G_TraceLog()->sysLog(8, "66don't put this user");
    }
    else
    {
        u->IncPendingSendNum();
        u->SetSending(true);
        if (msg->GetWouldBlock())
        {
            u->PushWouldBlockMessage(msg);
        }
        else
        {
            u->PushSendMessage(msg);
        }
        sendQueue.push(u);
        mQueueSize = mQueueSize + 1;
        pthread_cond_signal(&isEmpty);
        pthread_mutex_unlock(&senderLock);
        G_TraceLog()->sysLog(8, "out PushSendUser");
    }
