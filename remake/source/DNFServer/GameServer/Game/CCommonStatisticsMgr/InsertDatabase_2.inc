void CPVPMapCount::InsertDatabase()
{
    if (m_quantityMap.size() == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x538),
                       true);
    **guard << 0x105;
    **guard << -1;
    **guard << (int)m_quantityMap.size();
    for (std::map<int, int>::iterator it = m_quantityMap.begin(),
                                      endIt = m_quantityMap.end();
         it != endIt; ++it)
    {
        **guard << it->first;
        **guard << it->second;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
