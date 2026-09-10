int COnlinePreliminaryTeam::_saveTeamMatchList()
{
    if (!m_hasLoseFlag)
        return 1;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x4e),
        false);
    guard.operator*()->operator<<(0x1ed);
    guard.operator*()->operator<<(-1);

    SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST* p =
        guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST>();
    p->m_teamId = m_field04;
    p->m_count = 0;

    for (std::map<int, PlayCountInfoEx>::iterator it = m_playCountMap.begin();
         it != m_playCountMap.end(); ++it)
    {
        PlayCountInfoEx& info = it->second;
        if (!info.m_flag)
            continue;
        info.m_flag = 0;

        p->m_entries[p->m_count].m_otherTeamId = it->first;
        p->m_entries[p->m_count].m_playCount = info.m_count;
        p->m_count += 1;

        if (p->m_count == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            guard.Set(GlobalData::s_stream_pool->Acquire(
                "OnlinePreliminaryTeam.cpp", 0x66));
            // ORIG 0x085881a0..0x085881de：Set 后重写流头（<<0x1ed、<<-1）
            // 再经 operator-> 取新缓冲（P103 调用链不可省）。
            guard.operator*()->operator<<(0x1ed);
            guard.operator*()->operator<<(-1);
            p = guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST>();
            p->m_teamId = m_field04;
            p->m_count = 0;
        }
    }

    if (p->m_count != 0)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    else
        GlobalData::s_stream_pool->Free(guard);
    m_hasLoseFlag = 0;
    return 1;
