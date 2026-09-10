void CDeathMatchBattleMgr::checkWinnerForTeamPlay(
    CUser** users, bool* win, unsigned char* teamWin, PvpUserTable& table)
{
    int killTeam1 = 0;
    int killTeam2 = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0)
        {
            if (teamWin[i] == 1)
                killTeam1 += m_killCount[i];
            else if (teamWin[i] == 2)
                killTeam2 += m_killCount[i];
            else
                LogManager::logFormat(
                    1, "pvp.cpp",
                    __PRETTY_FUNCTION__,
                    0x1617,
                    "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                    i, (unsigned int)teamWin[i]);
        }
    }
    // [本波·R20 建议] ORIG 0x085df296 用 `int winCount[2]`（基址 -0x24 + team 索引）
    // 而非两个独立标量 ⇒ 改数组形（索引访问形态与 ORIG 的 `-0x24(,%edx,4)` 一致）。
    int aliveTeam[2] = {0, 0};
    for (int i = 0; i <= 7; ++i)
    {
        if (win[i] && users[i] != 0)
        {
            int t = teamWin[i] - 1;
            if (t >= 0 && t <= 1)
                ++aliveTeam[t];
        }
    }
    unsigned char winTeam;
    if (killTeam1 < killTeam2)
        winTeam = 2;
    else if (killTeam1 > killTeam2)
        winTeam = 1;
    else if (aliveTeam[0] > aliveTeam[1])
        winTeam = 1;
    else if (aliveTeam[1] > aliveTeam[0])
        winTeam = 2;
    else
        winTeam = (rand() & 1) ? 2 : 1;
    memset(win, 0, 8);
    for (int i = 0; i <= 7; ++i)
    {
        if (teamWin[i] == winTeam)
            win[i] = true;
    }
    table.SetWinTeam((int)winTeam);
