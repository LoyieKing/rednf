bool COnlinePreliminaryMember::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp", 0x21),
        true);
    guard.operator*()->operator<<(0x1e7);
    guard.operator*()->operator<<(static_cast<int>(pUser->GetUID()));

    SIG_ONLINE_PRELIMINARY_MEMBER* p =
        guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_MEMBER>();
    p->m_serverGroup = static_cast<unsigned short>(pUser->GetServerGroup());
    p->m_characNo = pUser->getCurCharacNo();
    p->m_rating = m_rating;   // ORIG movl 4B 直拷
    p->m_winCount = m_winCount;
    p->m_loseCount = m_loseCount;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
