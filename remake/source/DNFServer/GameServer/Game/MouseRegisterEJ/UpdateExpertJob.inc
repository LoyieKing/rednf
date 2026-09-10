void CExpertJobStatistic::UpdateExpertJob()
{
    if (m_counts.empty())
    {
        return;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x65a), true);
    guard.operator*()->operator<<(0x182);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(static_cast<int>(m_counts.size()));
    // 判据⑩/P326：ORIG 0x0860fba7 在循环外构造 end 迭代器（-0x20），循环条件只调 ne；
    // 内联 `it != m_counts.end()` 会被 4.4 搬进条件块每轮重算并占浅槽 -0x10
    // （ORIG 该槽是 begin() 的返回临时）⇒ 具名 end（声明在 it 之后，保持 begin→end 构造序）。
    std::map<ExpertJobKey, int>::const_iterator it = m_counts.begin();
    std::map<ExpertJobKey, int>::const_iterator itEnd = m_counts.end();
    for (; it != itEnd; ++it)
    {
        guard.operator*()->operator<<(it->first.m_field00);
        guard.operator*()->operator<<(static_cast<int>(it->first.m_itemIdx04));
        guard.operator*()->operator<<(it->second);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_counts.clear();
