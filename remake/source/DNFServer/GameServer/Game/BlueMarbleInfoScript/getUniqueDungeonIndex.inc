std::vector<int> BlueMarbleInfoScript::getUniqueDungeonIndex(int level)
{
    std::vector<int> result;
    unsigned int i = 0;
    while (i < m_uniqueDungeonList.size())
    {
        BlueMarbleRandomDungeonInfo& elem = m_uniqueDungeonList[i];
        if (elem.m_minLevel <= level && level <= elem.m_maxLevel)
        {
            result.push_back(elem.m_dungeonIndex);
        }
        ++i;
    }
    return result;
