void DataPool::destroyTCPUser(TCPUser* pTCPUser)
{
    if (pTCPUser->isBindedSession())
    {
        G_TraceLog()->sysLog(8, "destroy!!! isSending=%d, rp-%d, sp-%d, wc-%d",
            pTCPUser->IsSending(),
            pTCPUser->GetPendingWorkNum(),
            pTCPUser->GetPendingSendNum(),
            pTCPUser->getWorkThread()->orderQueue.size());
    }
    else
    {
        G_TraceLog()->sysLog(8, "destroy!!! isSending=%d, rp-%d, sp-%d",
            pTCPUser->IsSending(),
            pTCPUser->GetPendingWorkNum(),
            pTCPUser->GetPendingSendNum());
    }
    if (pTCPUser->IsSending())
    {
        __int64 curTime = time(NULL);
        if (pTCPUser->DisLast_ == 0)
        {
            pTCPUser->DisLast_ = curTime;
            pTCPUser->bDisconnected_ = false;
            return;
        }
        if (curTime - pTCPUser->DisLast_ < 0x78)
        {
            pTCPUser->bDisconnected_ = false;
            return;
        }
    }
    G_TraceLog()->sysLog(8, "destroy!!!aaa");
    pTCPUser->ClearRecvMsgs();
    G_TraceLog()->sysLog(8, "destroy!!!bbb");
    if (pTCPUser->getSession() != NULL)
    {
        pTCPUser->getSession()->onClose(false);
        pTCPUser->setSession(NULL);
    }
    TCPSocket* sock = pTCPUser->getSocket();
    if (sock != NULL)
    {
        EpollReactor<TCPUser>* pReactor = pApp->super_Reactor.getReactor();
        pReactor->unregistHandle(pTCPUser);
        sock->close();
        destroyTCPSocket(sock);
    }
    pTCPUser->setSocket(NULL);
    pTCPUser->SetSending(false);
    pTCPUser->bDisconnected_ = true;
    pTCPUser->~TCPUser();
    TCPUserPool->free(pTCPUser);
    mTcpUserCount = mTcpUserCount - 1;
    G_TraceLog()->sysLog(3, "destroyTCPUser(): Total Con = %d", GetTcpUserCount());
