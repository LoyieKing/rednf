// ---- makeRequest ----
// ORIG 0x816179c：Acquire stream(0x1a8) → CStreamGuard(stream,true) →
// internal_stream(guard, 0xd, uid) → guard->GetInBuffer<Msg>()；
// 写 m_id(0xc)/remainCount(0x10 char) 入 MsgQueueMgr::put(1, guard)；
// msg==0 时 logFormat "[Taiwan, pcroom] m_id:%d" 后 return。
// 注意：__PRETTY_FUNCTION__ 实证第二参类型名为 memberIdentificationNumber_t（=unsigned int）。
void InterSelectPcroomDailyReward::makeRequest(int uid, memberIdentificationNumber_t m_id,
                                               char remainCount)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x1a8),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xd), uid);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x1ad, "[Taiwan, pcroom] m_id:%d", m_id);
    }
    else
    {
        msg->m_id = m_id;
        msg->m_remainCount = remainCount;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}