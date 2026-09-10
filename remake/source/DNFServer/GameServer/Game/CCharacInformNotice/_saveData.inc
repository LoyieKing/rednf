bool CCharacInformNotice::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacInformNotice.cpp", 0x77), true);
    // ORIG（idx51 报告）：每次 << 先 operator*()（de）取流再 operator<<；GetUID
    // 先求值（ORIG +0x057 call GetUID → +0x064 operator* → +0x076 operator<<）。
    guard.operator*()->operator<<(0x311);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_SAVE_CHARAC_INFORM_NOTICE* sig =
        guard->GetInBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>();
    if (sig == 0)
        return false;
    memset(sig, 0, 0x14);
    sig->m_characNo = user->get_charac_no(-1);
    memcpy(sig->m_flags, m_flags, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
