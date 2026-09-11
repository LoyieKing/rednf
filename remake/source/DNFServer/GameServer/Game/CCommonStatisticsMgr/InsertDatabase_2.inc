// ---- CPVPMapCount::InsertDatabase（ORIG 0x0860eb16 T）----
// pool 流 0x538、msg 0x105：-1 后写整表 size，再遍历输出全部 (key,value)。
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
}