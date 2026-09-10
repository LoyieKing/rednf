int CDataManager::getGiveGrowCreatureEventJobCount(int level)
{
    if (m_createCreatureGift.m_eventJobMap.empty())
    {
        return 0;
    }
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator it =
        m_createCreatureGift.m_eventJobMap.begin();
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator e =
        m_createCreatureGift.m_eventJobMap.end();
    int count = 0;
    while (it != e)
    {
        if (it->second.m_minLevel < level && level < it->second.m_maxLevel)
        {
            count += 1;
        }
        it++;
    }
    return count;
