// ==== ORIG 085defcc ====
int CDeathMatchBattleMgr::getMaxKillCount()
{
    int max = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (m_killCount[i] > max)
            max = m_killCount[i];
    }
    return max;
}