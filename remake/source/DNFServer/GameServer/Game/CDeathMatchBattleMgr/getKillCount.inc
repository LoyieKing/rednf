int CDeathMatchBattleMgr::getKillCount(int userIdx)
{
    if (userIdx <= 7)
        return m_killCount[userIdx];
    return 0;
