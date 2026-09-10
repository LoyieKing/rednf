void CConditionEventManager::ConditionRewardSameItemCombine(
    std::vector<stConditionEventRewardInfo>& infos, std::map<int, int>& outMap)
{
    std::vector<stConditionEventRewardInfo>::iterator it = infos.begin();
    for (; it != infos.end(); ++it)
    {
        std::map<int, int>::iterator mit = outMap.find(it->m_field0);
        if (mit == outMap.end())
        {
            outMap.insert(std::make_pair(it->m_field0, it->m_field4));
        }
        else
        {
            mit->second += it->m_field4;
        }
    }
