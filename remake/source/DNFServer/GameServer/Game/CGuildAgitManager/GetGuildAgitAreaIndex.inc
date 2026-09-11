int CGuildAgitArea::GetGuildAgitAreaIndex(unsigned int guildKey, int start)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(guildKey);
    if (it != m_areaMap.end())
    {
        return it->second - start;
    }
    return -1;
}