// ==== ORIG 085df48a ====
void CDeathMatchBattleMgr::checkWinnerForSinglePlay(
    CUser** users, bool* win, PvpUserTable& table)
{
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] == 0)
            m_killCount[i] = 0;
        win[i] = false;
    }
    int* pMax = std::max_element(m_killCount, m_killCount + 8);
    int winIdx = (int)(((unsigned int)pMax - (unsigned int)m_killCount) >> 2);
    if (winIdx <= 7)
        win[winIdx] = true;
    table.SetWinTeam(winIdx);
}