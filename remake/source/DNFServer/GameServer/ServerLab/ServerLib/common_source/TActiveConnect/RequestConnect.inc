TCPUser* ActiveConManager::RequestConnect(ConInterface* conInfo)
{
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        queueRequestConnect.push(conInfo);
        bRequestInQueue = true;
    }
    while (true)
    {
        ConInterface* rConInfo;
        if (bConnectedInQueue != true)
        {
            if (conInfo->mIsRejected != false)
            {
                break;
            }
            TSystem<LinuxSystem>::sleep(10);
            continue;
        }
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockOutCon);
            if (mapConnectedUser_.empty())
            {
                bConnectedInQueue = false;
                TSystem<LinuxSystem>::sleep(10);
                continue;
            }
            for (TCPUserConnectMap::iterator iter = mapConnectedUser_.begin();
                 iter != mapConnectedUser_.end(); iter++)
            {
                rConInfo = iter->second;
                printf("rConInfo->getId-%d\n", rConInfo->getId());
                if (conInfo->getId() == rConInfo->getId())
                {
                    conInfo->setTCPUser(rConInfo->getTCPUser());
                    mapConnectedUser_.erase(iter);
                    return conInfo->getTCPUser();
                }
            }
        }
        TSystem<LinuxSystem>::sleep(100);
    }
    return NULL;
