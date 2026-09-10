int ServiceFactory::startup()
{
    int ret;
    puts("\xbc\xad\xb9\xf6 \xb6\xf3\xc0\xcc\xba\xea\xb7\xaf\xb8\xae \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb...");
    printf("%d\n", (int)super_IHandlers.mInterHandlers.size());
    ret = 0;
    // ORIG: 赋值在条件内（mov n; shr $0x1f; test; je），失败提前 return。
    if ((ret = G_TraceLog()->init(ServiceFactoryName, ServiceConfigName)) < 0)
    {
        return ret;
    }
    super_DataPools.dataPool_ = new DataPool;
    super_PortInfo.setUDPPort(G_Script()->findIntValue(0, 5));
    super_PortInfo.setTCPPort(G_Script()->findIntValue(0, 4));
    TCPSocket::SetRecvBufSize(G_Script()->findIntValue(0, 10));
    TCPSocket::SetSendBufSize(G_Script()->findIntValue(0, 0xb));
    printf("%d\n", (int)super_IHandlers.mInterHandlers.size());
    super_Threads.threadTCPSend_[0] = new TCPSendThread;
    puts("TCPSendThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    super_IActiveConManager.activeConManager_ = new ActiveConManager;
    puts("TimerThread \xb0\xb4\xc3\xbc \xbb\xfd\xbc\xba...");
    super_Threads.threadTimer_ = new TimerThread;
    puts("TimerThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    G_TraceLog()->setLogSendThread(super_Threads.logSendThread_);
    puts("TCPThread \xb0\xb4\xc3\xbc \xbb\xfd\xbc\xba...");
    super_Threads.threadTCP_ = new TCPThread;
    super_Threads.threadTCP_->setPort(super_PortInfo.getTCPPort());
    puts("TCPThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    puts("NSLDBThread \xb0\xb4\xc3\xbc \xbb\xfd\xbc\xba...");
    super_Threads.threadDB_[0] = new NSLDBThread;
    puts("NSLDBThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    // ORIG：plain `new`（ctor 体无调用不可抛 → 无 EH pads），
    // ebx→eax 传参、ebx→edx 存成员（v5 形态）。
    super_Dispatchers.dispatcherTCP = new TCPDispatcher;
    puts("TCPDispatcher has been created...");
    super_Dispatchers.mpInterDispatcher = new InterDispatcher;
    puts("InterDispatcher has been created...");
    super_Dispatchers.dispatcherDB = new DBDispatcher;
    puts("DBDispatcher \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    puts("\xbc\xad\xba\xf1\xbd\xba \xbe\xb2\xb7\xb9\xb5\xe5 \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb");
    G_TraceLog()->sysLog(0, "\xbc\xad\xba\xf1\xbd\xba \xbe\xb2\xb7\xb9\xb5\xe5 \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb");
    super_Threads.mWorkThreadNum = 1;
    for (int i = 0; i < super_Threads.mWorkThreadNum; i = i + 1)
    {
            super_Threads.threadWork_[i] = new WorkThread(i);
        super_Threads.threadWork_[i]->begin();
        printf("%d\xb9\xf8 workThread \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8\n", i);
    }
    TSystem<LinuxSystem>::sleep(2000);
    printf("NSLDBThread 0 \xb9\xf8 \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb-%p\n",
           super_Threads.threadDB_[0]);
    super_Threads.threadDB_[0]->begin();
    G_TraceLog()->sysLog(0, "NSLDBThread 0 \xb9\xf8 \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p",
                         super_Threads.threadDB_[0]);
    super_Threads.threadTCPSend_[0]->begin();
    G_TraceLog()->sysLog(0, "TCPSendThread 0 \xb9\xf8 \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p",
                         super_Threads.threadTCPSend_[0]);
    super_Threads.threadTimer_->begin();
    printf("TimerThread \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p\n", super_Threads.threadTimer_);
    {
        std::map<int, INetWorkHandler*>::iterator iter;
        iter = super_IHandlers.mNetWorkHandlers.begin();
        for (; iter != super_IHandlers.mNetWorkHandlers.end(); ++iter)
        {
            iter->second->init();
        }
    }
    {
        std::map<int, IDBHandler*>::iterator iter;
        iter = super_IHandlers.mDBHandlers.begin();
        iter->second->init();
    }
    {
        std::map<int, IInterHandler*>::iterator iter;
        if (!super_IHandlers.mInterHandlers.empty())
        {
            iter = super_IHandlers.mInterHandlers.begin();
            iter->second->init();
        }
    }
    // ORIG：循环遍历 IHandlers::mTimeHandlers[]（@0x104），虚调用 vtable[2]
    // （init，无参）；不是 threadWork_（@0x24）。
    for (int i = 0; i < super_IHandlers.mTimeHandlerNum; i = i + 1)
    {
        super_IHandlers.mTimeHandlers[i]->init();
    }
    super_Threads.threadTCP_->begin();
    printf("TCPThread \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p\n", super_Threads.threadTCP_);
    puts("------------------------------------------");
    puts("-\t\tServer Frame Start Ver1.0       -");
    puts("------------------------------------------");
    G_TraceLog()->sysLog(5, "----------------------------------------------------------");
    G_TraceLog()->sysLog(5, "-\t\tServer Frame Start Ver1.0 \t\t\t\t    -");
    G_TraceLog()->sysLog(5, "----------------------------------------------------------");
    if (G_TraceLog()->checkLogServer())
    {
        G_TraceLog()->sysLog(0, "checkLogServer(), write_to_logserver is true");
    }
    else
    {
        G_TraceLog()->sysLog(0, "checkLogServer(), write_to_logserver is false");
    }
    return ret;
