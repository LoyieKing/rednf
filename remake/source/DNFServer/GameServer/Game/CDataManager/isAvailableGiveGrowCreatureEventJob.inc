bool CDataManager::isAvailableGiveGrowCreatureEventJob(int job, int level)
{
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator
        it = m_createCreatureGift.m_eventJobMap.find(job);
    if (it != m_createCreatureGift.m_eventJobMap.end())
    {
        if (it->second.m_minLevel < level && level < it->second.m_maxLevel)
        {
            return true;
        }
    }
    return false;
}