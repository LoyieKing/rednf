// ---- makeRequest ----
// ORIG T 0x08161218：Acquire stream → CStreamGuard(stream,true) →
// internal_stream(guard, 0xa, uid) → guard->GetInBuffer<Msg>()；
// 写 m_id/received 入 MsgQueueMgr::put(1, guard)。
// [N1F 2026-09-11 §6.5 串盲区] 同 makeRequest.inc：ORIG prettiness 串为
//   `static void InterSelectCreateDnfUserInfo::makeRequest(int, memberIdentificationNumber_t, bool)`
//   （strall 审计实读；我方原写 `unsigned int`）。mangled `...Eijb` 不变。
typedef unsigned int memberIdentificationNumber_t;
void InterSelectCreateDnfUserInfo::makeRequest(int uid, memberIdentificationNumber_t memberId,
                                               bool received)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x14d),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xa), uid);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x152,
                              "[Taiwan, Event] m_id:%d isReceived:%d",
                              memberId, received);
    }
    else
    {
        msg->m_id = memberId;
        msg->m_received = received;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}