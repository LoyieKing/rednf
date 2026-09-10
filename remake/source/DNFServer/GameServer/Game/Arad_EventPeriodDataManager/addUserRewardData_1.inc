void Arad_EventPeriodDataManager::addUserRewardData(
    CUser* user, EventRewardItemArad& item)
{
    (void)user;
    Guard<Mutex> guard(&m_mutex);
    m_rewardItemList.push_back(item);
