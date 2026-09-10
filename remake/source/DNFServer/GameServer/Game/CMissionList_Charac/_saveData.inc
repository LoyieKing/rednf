bool CMissionList_Charac::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PvP_MissionSystem.cpp", 0x4a5),
        true);
    **guard << 0x263;
    **guard << (int)pUser->GetUID();
    SIG_REQUEST_PVP_MISSION_LIST* pData =
        guard->GetInBuffer<SIG_REQUEST_PVP_MISSION_LIST>();
    if (!pData)
    {
        return 0;
    }
    std::memset(pData, 0, 0x14a);
    pData->m_characNo = pUser->get_charac_no(-1);
    for (int kind = 0; kind < 36; ++kind)
    {
        pData->m_missionList[kind].m_index =
            m_missionList[kind].m_index;
        pData->m_missionList[kind].m_kind =
            m_missionList[kind].m_kind;
        pData->m_missionList[kind].m_count =
            m_missionList[kind].m_count;
        pData->m_missionList[kind].m_streak =
            m_missionList[kind].m_streak;
    }
    // ORIG +0x14f：BitsetToStr(m_clearedFlags, m_cleared) 直接写 unsigned char*
    //（Ph 重载），无 std::string 中间层（原实现错误引出了 string ctor/dtor/data）。
    BitsetToStr(m_clearedFlags, pData->m_cleared);
    pData->m_field144 = m_field148;
    pData->m_field146 = m_field14a;
    pData->m_field148 = m_field14c;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
