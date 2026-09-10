void ActiveNetClose::pushActiveClose(TCPUser* pUser)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLock);
    register unsigned int id = pUser->getUserId();
    activeCloseMap[id] = pUser;
    bInData = true;
    G_TraceLog()->sysLog(7, "active close push user tcpuserid :(%d) map size:(%d)", id, activeCloseMap.size());
