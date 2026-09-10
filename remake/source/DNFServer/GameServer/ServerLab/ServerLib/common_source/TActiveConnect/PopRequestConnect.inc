bool ActiveConManager::PopRequestConnect(TCPUser*& outConnectedUser)
{
    ConInterface* conInfo;
    timeval tv;
    register bool bRet;
    register int bExist;
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        if (queueRequestConnect.empty())
        {
            bRequestInQueue = false;
            bRet = false;
            bExist = 0;
        }
        else
        {
            conInfo = queueRequestConnect.front();
            queueRequestConnect.pop();
            bExist = 1;
        }
    }
    if (bExist)
    {
        TCPSocket* sTCP = pApp->super_DataPools.getDataPool()->createTCPSocket();
        if (sTCP->open() == false)
        {
            puts("failed to open UDP socket port");
            pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
            bRet = false;
            goto END;
        }
        printf("try to connect-%s, %d\n", conInfo->getIp(), conInfo->getPort());
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        // ORIG: fail branch first via xor $1 on connect result
        if (sTCP->connect_nonb(conInfo->getIp(), (unsigned short)conInfo->getPort(), tv) == false)
        {
            G_TraceLog()->sysLog(7, "failed to connect remote server-ip=%s, port=%d", conInfo->getIp(), conInfo->getPort());
            sTCP->close();
            pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
            conInfo->mIsRejected = true;
            bRet = false;
            goto END;
        }
        conInfo->SetConnected(true);
        printf("connection success!!-%s, %d\n", conInfo->getIp(), conInfo->getPort());
        TCPUser* acUser = pApp->super_DataPools.getDataPool()->createTCPUser();
        if (acUser == NULL)
        {
            sTCP->close();
            pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
            puts("cannot create TCP USER");
            bRet = false;
            goto END;
        }
        acUser->initialize();
        acUser->startupAfterSetSocket();
        acUser->setSocket(sTCP);
        acUser->setLastAccessTime();
        acUser->setNeedReconnect(conInfo->getNeedRecon());
        acUser->setSendDataType(conInfo->getSendType());
        acUser->setRecvDataType(conInfo->getRecvType());
        G_TraceLog()->sysLog(8, "Active Connected TCPUser id=[%d]", acUser->mUserId);
        TCPReactor* r = pApp->super_Reactor.getReactor();
        if (r->registHandle(acUser, 5) == false)
        {
            puts("register handle fail");
            bRet = false;
            goto END;
        }
        conInfo->setTCPUser(acUser);
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockOutCon);
            mapConnectedUser_[conInfo->getId()] = conInfo;
            bConnectedInQueue = true;
            printf("con map size-%d\n", (int)mapConnectedUser_.size());
        }
        outConnectedUser = acUser;
        bRet = true;
    }
END:
    return bRet;
