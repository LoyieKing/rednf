// ORIG T 0x0860faca（UpdateExpertJob.md）：map 空直接返回；否则 Acquire
// ("Statistics.cpp", 0x65a) 后写头 0x182/-1/map.size()，遍历逐条写
// {key.m_field00, key.m_itemIdx04, value}，put(DB_Q) 后 clear()。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x0860faca orig/ours=137/137
//   残差：槽/寄存器分配跨版本差（指令数全等，语义对齐）。
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
}