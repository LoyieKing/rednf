bool CLevelRewardTableMgr::IsHaveStartLevelReward()
{
    int i = 0;
    while (i <= 0x45)
    {
        if (m_table->m_levelKeys[i] == 0)
            return false;
        if (m_table->m_levelKeys[i] == 1)
            return true;
        ++i;
    }
    return false;
