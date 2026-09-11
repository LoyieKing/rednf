// ==== ORIG 085def0e ====
void CDeathMatchBattleMgr::IncreaseKillCount(int userIdx)
{
    if (userIdx >= 0 && userIdx <= 7)
        ++m_killCount[userIdx];
}