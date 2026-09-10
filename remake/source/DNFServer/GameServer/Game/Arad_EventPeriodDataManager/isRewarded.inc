bool Arad_EventPeriodDataManager::isRewarded(EventRewardItemArad item)
{
    Guard<Mutex> guard(&m_mutex);
    return std::find(m_rewardItemList.begin(), m_rewardItemList.end(), item) !=
           m_rewardItemList.end();
