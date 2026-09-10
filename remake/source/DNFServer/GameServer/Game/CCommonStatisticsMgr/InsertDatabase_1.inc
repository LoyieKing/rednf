void CPVPPlayerRate::InsertDatabase()
{
    if (m_quantityMap.size() == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x525),
                       true);
    **guard << 0x104;
    **guard << -1;
    for (int i = 0; i <= 5; ++i)
        **guard << m_quantityMap[i];
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
