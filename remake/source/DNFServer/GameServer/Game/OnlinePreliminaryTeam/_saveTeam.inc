bool COnlinePreliminaryTeam::_saveTeam()
{
    if (!m_hasWinFlag)
        return true;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x38),
        true);
    guard.operator*()->operator<<(0x1e9);
    guard.operator*()->operator<<(-1);

    // 具名字段写入（布局证据 SigTypes_D.h::SIG_ONLINE_PRELIMINARY_TEAM）：
    // +0 teamId / +0x14 rating 位 / +0x18 win / +0x1c lose，ORIG 逐条 movl。
    SIG_ONLINE_PRELIMINARY_TEAM* t =
        guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM>();
    t->m_teamId = m_field04;
    t->m_rating = m_rating08;
    t->m_winCount = m_winCount;
    t->m_loseCount = m_loseCount;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_hasWinFlag = 0;
    return true;
