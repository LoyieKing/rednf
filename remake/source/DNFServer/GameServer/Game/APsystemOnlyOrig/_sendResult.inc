// ============================================================================
// _sendResult @ 0x08421e3a
// StreamPool::Acquire("DBThread.cpp", 0x5a14) → CStreamGuard → << 0x9a / << -1 →
// GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG> 写 +0x00=mid → put(DISPATCH_Q)。
// ============================================================================
void DB_SaveClientHackLog::_sendResult(unsigned int mid)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5a14), true);
    guard.operator*()->operator<<(0x9a);   // SIG_SAVE_CLIENT_HACK_LOG
    guard.operator*()->operator<<(-1);

    SIG_SAVE_CLIENT_HACK_LOG* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>();
    *reinterpret_cast<unsigned int*>(sig) = mid;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}