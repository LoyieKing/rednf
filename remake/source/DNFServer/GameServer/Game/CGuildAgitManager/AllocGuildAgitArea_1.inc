void CGuildAgitManager::AllocGuildAgitArea(unsigned int guildKey)
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_guildCountMap.find(guildKey);
    if (it != m_guildCountMap.end())
    {
        it->second += 1;
    }
    else
    {
        m_guildCountMap.insert(std::make_pair(guildKey, 1));
        m_area.AllocGuildAgitArea(guildKey);
    }
}