void DB_CancelRestingUserRestrict::makeRequest(unsigned int uid, int param2)
{
    // ORIG T 0x0843741e：Acquire(__PRETTY_FUNCTION__,0x8389) → <<0x195 <<param2
    // → GetInBuffer<SIG_RESTING_USER_RESTRICT> 写 uid@+0 → put DB_Q。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(__PRETTY_FUNCTION__, 0x8389), true);
    **guard << 0x195;
    **guard << param2;
    SIG_RESTING_USER_RESTRICT* sig =
        guard->GetInBuffer<SIG_RESTING_USER_RESTRICT>();
    sig->m_uid = uid;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
