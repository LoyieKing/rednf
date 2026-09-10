bool COnlinePreliminaryMemberMatchList::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp", 0x82),
        true);
    guard.operator*()->operator<<(0x1eb);
    guard.operator*()->operator<<(static_cast<int>(pUser->GetUID()));

    SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST* p =
        guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST>();
    p->m_serverGroup = static_cast<unsigned short>(pUser->GetServerGroup());
    p->m_characNo = static_cast<unsigned int>(pUser->getCurCharacNo());
    p->m_count = 0;

    for (std::map<CharacterInServer, PlayCountInfo>::iterator it =
             m_peerMap.begin();
         it != m_peerMap.end(); ++it)
    {
        const CharacterInServer& key = it->first;
        PlayCountInfo& info = it->second;
        if (!info.m_flag)
            continue;
        info.m_flag = 0;

        p->m_entries[p->m_count].m_serverId =
            static_cast<unsigned short>(key.m_serverId);
        p->m_entries[p->m_count].m_uniqueId = key.m_uniqueId;
        p->m_entries[p->m_count].m_playCount = info.m_count;
        p->m_count += 1;

        if (p->m_count == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            // 分批重建：新流临时守卫；写完 header 后指针切至其 InBuffer，
            // 临时守卫随即析构（对应 ORIG D1 @0x08586777）。
            CStreamGuard tmp(
                GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp",
                                                   0x9d),
                true);
            tmp.operator*()->operator<<(0x1e7);
            tmp.operator*()->operator<<(static_cast<int>(pUser->GetUID()));
            p = tmp->GetInBuffer<SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST>();
            p->m_serverGroup =
                static_cast<unsigned short>(pUser->GetServerGroup());
            p->m_characNo =
                static_cast<unsigned int>(pUser->getCurCharacNo());
            p->m_count = 0;
        }
    }

    if (p->m_count != 0)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    else
        GlobalData::s_stream_pool->Free(guard);
    return true;
