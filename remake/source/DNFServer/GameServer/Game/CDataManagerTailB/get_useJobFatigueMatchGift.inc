int CDataManager::get_useJobFatigueMatchGift(char job, int a, int b)
{
    int jobType;
    const int diff = a - b;
    jobType = (int)job;
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find(jobType);
    if (it != m_useJobFatigue.m_giftMap.end())
    {
        std::map<int, int>::iterator u1 =
            it->second.m_rewardMap.upper_bound(diff);
        std::map<int, int>::iterator u2 =
            it->second.m_rewardMap.upper_bound(diff + b);
        if (u1 == it->second.m_rewardMap.end() &&
            u2 == it->second.m_rewardMap.end())
        {
            return 0;
        }
        if (u1 != u2)
        {
            return u1->second;
        }
    }
    return 0;
