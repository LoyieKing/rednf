// ==== ORIG 085def34 ====
void CDeathMatchBattleMgr::IncreaseDeathCount(int userIdx)
{
    if (userIdx >= 0 && userIdx <= 7)
        ++m_deathCount[userIdx];
}