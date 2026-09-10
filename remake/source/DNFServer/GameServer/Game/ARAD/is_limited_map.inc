bool Arad_OnePlusCardEvent_ItemList::is_limited_map(unsigned int mapIndex)
{
    std::vector<unsigned int>::iterator it = std::find(
        m_limitedMap.begin(), m_limitedMap.end(), mapIndex);
    if (it == m_limitedMap.end())
    {
        return 0;
    }
    return 1;
