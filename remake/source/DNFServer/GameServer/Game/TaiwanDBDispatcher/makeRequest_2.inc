// ORIG 0x0815c25c：GetRepeatEvent(0xa0) 非空且 IsEventing((CUser*)0)==1 才
// 发消息：Acquire(file, 0x261) → CStreamGuard(true) → internal_stream(guard,
// 0xb, a) → GetInBuffer<MSG> null → cMyTrace(0x267)("[Taiwan, Event] msg is
// null")；否则 *buf = b；MsgQueueMgr::put(DB_Q, guard)。
// [b28d-lane6] AE（已修）：去 bool skip 汇聚槽改早期返回 + P221 临时链（复现 ORIG 0x0815c25c 帧 0x30/槽序）。
// [P2S5] P139 正向缺口：ORIG prett 串（0x8b78fe0, size=0x56）第三参拼写为
// `memberIdentificationNumber_t`（typedef unsigned int，GameTypes.h，经
// TaiwanQueries.h 可见）⇒ mangled 不变（j），零 ABI。
void DBInsertCreateDnfUserInfo::makeRequest(int a,
                                            memberIdentificationNumber_t b)
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
}
