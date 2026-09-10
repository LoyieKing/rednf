bool ConditionLevelChkDungeon::isApplyLevel(int level)
{
    if (!(m_applyLevel > level))
    {
        return true;
    }
    return false;
