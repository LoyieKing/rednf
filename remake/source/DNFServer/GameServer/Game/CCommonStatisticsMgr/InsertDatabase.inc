// ---- CHelpedPartyCount::InsertDatabase（ORIG 0x0860e8aa T）----
// pool 流 0x512、msg 0x103、固定遍历键 0..4。
void CHelpedPartyCount::InsertDatabase()
{
    if (m_quantityMap.size() == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x512),
                       true);
    **guard << 0x103;
    **guard << -1;
    for (int i = 0; i <= 4; ++i)
        **guard << m_quantityMap[i];
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
}