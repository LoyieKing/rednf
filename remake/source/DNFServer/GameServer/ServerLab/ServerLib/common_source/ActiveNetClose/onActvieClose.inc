bool ActiveNetClose::onActvieClose(unsigned int& outId, TCPUser*& outUserPtr)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLock);
    std::map<unsigned int, TCPUser*>::iterator iter = activeCloseMap.begin();
    register unsigned int key = iter->first;
    TCPUser* pUser = iter->second;
    bool ret_val = false;
    if (pUser->onActiveClose(key))
    {
        activeCloseMap.erase(key);
        outId = key;
        outUserPtr = pUser;
        ret_val = true;
        G_TraceLog()->sysLog(7, "onActiveClose erase tcpuserid :(%d) map size:(%d)", key, activeCloseMap.size());
    }
    if (activeCloseMap.size() == 0)
    {
        bInData = false;
    }
    else
    {
        bInData = true;
    }
    return ret_val;
