void DB_CheckSecuReward::makeRequest(int param1, unsigned int uid, int param3)
{
    // ORIG T 0x08444408：Acquire(__PRETTY_FUNCTION__,0xab34) → <<0x2ab <<param1
    // → GetInBuffer 写 uid@+0 param3@+4 → put DB_Q。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(__PRETTY_FUNCTION__, 0xab34), true);
    **guard << 0x2ab;
    **guard << param1;
    SIG_CHECK_USER_SECU_REWARD* sig =
        guard->GetInBuffer<SIG_CHECK_USER_SECU_REWARD>();
    reinterpret_cast<SIG_CSR_WRITEVIEW*>(sig)->m_uid = uid;
    reinterpret_cast<SIG_CSR_WRITEVIEW*>(sig)->m_param3 = param3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
