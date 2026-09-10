int CDataManager::get_GrowthWeaponEventReward(int idx) const
{
    std::map<int, int>::const_iterator it = m_growthWeapon.m_rewardMap.find(idx);
    if (it != m_growthWeapon.m_rewardMap.end())
    {
        return it->second;
    }
    return 0;
