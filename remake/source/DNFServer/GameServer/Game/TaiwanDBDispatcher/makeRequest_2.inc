void DBInsertCreateDnfUserInfo::makeRequest(int a, unsigned int b)
{
    // b28d-lane6 试形：去 bool skip 汇聚槽，改早期返回（ORIG 0x0815c25c 无 bool 槽）。
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    if (ev == 0 || !ev->IsEventing((CUser*)0))
        return;
    {
        // P89：单一视图表达式（合并 stream 局部）
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x261), true);
        Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0xb, a);
        DBInsertCreateDnfUserInfo::MSG* sig =
            guard.operator->()->GetInBuffer<DBInsertCreateDnfUserInfo::MSG>();
        if (sig == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x267, 0)("[Taiwan, Event] msg is null");
        }
        else
        {
            *reinterpret_cast<unsigned int*>(sig) = b;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
