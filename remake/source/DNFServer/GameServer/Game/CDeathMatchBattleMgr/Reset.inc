// ==== ORIG 085dee22 ====
void CDeathMatchBattleMgr::Reset()
{
    memset(m_killCount, 0, sizeof(m_killCount));
    memset(m_deathCount, 0, sizeof(m_deathCount));
}