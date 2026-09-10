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
