int SecretShopScript::GetLevelIdx(int level)
{
    for (std::set<stLevelSection>::iterator it = m_levelSections.begin();
         it != m_levelSections.end(); ++it) {
        if ((*it).m_minLev <= level && level <= (*it).m_maxLev)
            return (*it).m_levelIdx;
    }
    return 0;
