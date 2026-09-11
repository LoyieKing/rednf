// SendResult —— 报告全文转写。ORIG T 0x08124156。
bool DB_LoadActionPoint::SendResult(int a, int b, _SIG_LOAD_ACTION_POINT& sig, bool ok)
{
    if (ok)
    {
        // P0：ORIG 无 Stream* 中间局部 —— Acquire 返回值直传 CStreamGuard ctor
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0xb1), true);
        guard.operator*()->operator<<(a);
        guard.operator*()->operator<<(b);
        guard.operator->()->put_binary(&sig, 0xcf1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    else
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0xbc), true);
        guard.operator*()->operator<<(0x12);
        guard.operator*()->operator<<(b);
        guard.operator->()->put_binary(&sig, 0xcf1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
}