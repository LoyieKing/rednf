void CGuildAgitArea::ReleaseGuildAgitArea(unsigned int guildKey)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(guildKey);
    if (it != m_areaMap.end())
    {
        m_areaStack.push(it->second - 3);
        m_areaStack.push(it->second - 2);
        m_areaStack.push(it->second - 1);
        m_areaStack.push(it->second);
        m_areaMap.erase(it);
    }
