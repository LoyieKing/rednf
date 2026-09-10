int CharacManageScript::GetLevelUpRequirePoint(unsigned int maxLevel)
{
    if ((maxLevel == 0) || (maxLevel > m_manageLevelPoint.size()))
        return 0;
    return m_manageLevelPoint[maxLevel - 1];
