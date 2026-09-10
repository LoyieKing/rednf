int TimerQueryCounter::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // 第一段：LOGDB_Q(4)，值 0x7b / -1 —— ORIG 0x08635a60 段：Acquire 免槽直落
    // guard ctor、显式 operator*() 代理链、guard1 槽 -0x10 + {} 块界 dtor/catch
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x97b), true);
        guard.operator*()->operator<<((int)0x7b);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    // 第二段：DB_Q(2)，值 0x7c / -1（ORIG guard2 槽 -0x18 与 guard1 相邻）
    {
        CStreamGuard guard2(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x981), true);
        guard2.operator*()->operator<<((int)0x7c);
        guard2.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard2);
    }

    RegistNextTimer();
    return 1;
