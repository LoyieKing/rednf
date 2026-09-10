int CDeathMatchBattleMgr::getDeathCount(int userIdx)
{
    if (userIdx <= 7)
        return m_deathCount[userIdx];
    return 0;
