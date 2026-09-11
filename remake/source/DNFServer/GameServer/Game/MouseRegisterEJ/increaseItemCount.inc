// ORIG T 0x0860f9d4（increaseItemCount.md）：find 未命中则以 count 为初值插入，
// 否则 value += count。
void CExpertJobStatistic::increaseItemCount(ENUM_EXPERT_JOB_TYPE type,
                                            unsigned long itemIdx, int count)
{
    std::map<ExpertJobKey, int>::iterator it =
        m_counts.find(ExpertJobKey(type, itemIdx));
    if (it != m_counts.end())
    {
        it->second = it->second + count;
    }
    else
    {
        m_counts.insert(std::make_pair(ExpertJobKey(type, itemIdx), count));
    }
}