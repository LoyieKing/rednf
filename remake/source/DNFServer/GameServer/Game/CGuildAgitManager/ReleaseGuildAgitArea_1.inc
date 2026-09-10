void CGuildAgitManager::ReleaseGuildAgitArea(unsigned int guildKey)
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_guildCountMap.find(guildKey);
    if (it != m_guildCountMap.end())
    {
        it->second -= 1;
        if (it->second == 0)
        {
            m_guildCountMap.erase(it);
            m_area.ReleaseGuildAgitArea(guildKey);
        }
    }
