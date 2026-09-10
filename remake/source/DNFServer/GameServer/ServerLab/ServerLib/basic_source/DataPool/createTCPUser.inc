TCPUser* DataPool::createTCPUser()
{
    TCPUser* r = TCPUserPool->construct();
    if (r == NULL)
    {
        return NULL;
    }
    r->initialize();
    mTcpUserCount = mTcpUserCount + 1;
    G_TraceLog()->sysLog(3, "createTCPUser(): Total Con = %d", GetTcpUserCount());
    return r;
