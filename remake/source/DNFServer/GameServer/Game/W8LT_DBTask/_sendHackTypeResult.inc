// ============================================================================
// _sendHackTypeResult @ 0x08422152
// Acquire("DBThread.cpp", 0x5a6c) → guard → << 0x9a / << -1 →
// GetInBuffer 写 +0x00=mid、+0x08=(ushort)hackType、+0x0e=cnt → put(DISPATCH_Q)。
// ============================================================================
void DB_SaveClientHackLog::_sendHackTypeResult(unsigned int mid, int hackType,
                                               int cnt)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5a6c), true);
    guard.operator*()->operator<<(0x9a);   // SIG_SAVE_CLIENT_HACK_LOG
    guard.operator*()->operator<<(-1);

    SIG_SAVE_CLIENT_HACK_LOG_VIEW* v = reinterpret_cast<SIG_SAVE_CLIENT_HACK_LOG_VIEW*>(
        guard.operator->()->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>());
    v->m_mid = mid;
    v->m_hackType = static_cast<unsigned short>(hackType);
    v->m_cnt = cnt;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}