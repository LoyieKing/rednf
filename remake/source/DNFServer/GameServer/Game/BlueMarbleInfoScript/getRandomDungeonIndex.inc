std::vector<int> BlueMarbleInfoScript::getRandomDungeonIndex(int level)
{
    std::vector<int> result;
    unsigned int i = 0;
    while (i < m_randomDungeonList.size())
    {
        BlueMarbleRandomDungeonInfo& elem = m_randomDungeonList[i];
        if (elem.m_minLevel <= level && level <= elem.m_maxLevel)
        {
            result.push_back(elem.m_dungeonIndex);
        }
        ++i;
    }
    return result;
