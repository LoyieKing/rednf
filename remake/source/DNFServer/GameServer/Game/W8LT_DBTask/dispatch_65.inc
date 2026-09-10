bool DB_LoadNoCacheData::dispatch(int userIdx, int slot, Stream* stream)
{
    int characNo;
    SIG_LOAD_NO_CACHE_DATA* sig;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    characNo = 0;
    *stream >> characNo;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8549), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<SIG_LOAD_NO_CACHE_DATA>();
    if (sig == 0)
        return 0;
    memset(sig, 0, 0x2c);
    // P248 档③（b28d-4）：SIG 字段为 unsigned int 而 RHS 是 int 局部
    // （ORIG `Stream::rsERi` 实证）⇒ 4.4 恒插 `mov %eax,%edx` 转换中转；
    // ORIG 0x08437e.. store 为单段 RHS-first（无中转）。用本 TU int 视图
    // 与 RHS 同型化（布局同宽、sizeof/偏移不变）。
    sig->m_characNoInt = characNo;
    LoadBloodDungeonReward(sig);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
