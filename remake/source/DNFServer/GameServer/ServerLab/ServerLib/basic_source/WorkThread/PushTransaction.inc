void WorkThread::PushTransaction(IMessageStruct* pMessage)
{
    pthread_mutex_lock(&workerLock);
    if ((int)pMessage->mMsgType - 1U < 3)
    {
        orderQueue.push(pMessage);
    }
    else
    {
        TCPUser* u = ((Message*)pMessage)->getUserFromMessage();
        if (u->isAboutToDisconnect() || u->isDisconnected())
        {
            u->SetWorking(false);
            ((Message*)pMessage)->setUse(false);
            pthread_cond_signal(&isEmpty);
            pthread_mutex_unlock(&workerLock);
            G_TraceLog()->sysLog(8, "161don't put this user");
            return;
        }
        u->IncPendingWorkNum();
        u->SetWorking(true);
        orderQueue.push(pMessage);
        // ORIG（二进制实测）：`Message::ident` 作为单个 64 位 vararg 传入——
        // 一次双字装载进 ebx:esi，低/高字直接压栈（供 %d,%d 读取）。拆成两个
        // (int) 参数（含 register 局部）会让 GCC 4.4 -O0 产生重复装载+xor 舞步。
        G_TraceLog()->sysLog(4, "RECV PUSH USER=%x, ID=%d, msg=%d", u, u->mUserId, Message::ident);
    }
    mQueueSize = mQueueSize + 1;
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&workerLock);
