bool CDeathMatchBattleMgr::checkGameOverForTeamPlay(
    CUser** users, int teamCnt, unsigned char* teamWin)
{
    if (teamCnt == -1)
        return true;
    int killTeam1 = 0;
    int killTeam2 = 0;
    // [本波] ORIG 0x085df126 序言发三个零初始化 `movl $0x0,-0x18/-0x14/-0x10`
    // ⇒ maxKill 是**函数级具名 int（带 0 初值）**，三目赋值降为循环后的独立语句
    // （P28/P212 槽序：killTeam1 -0x18 → killTeam2 -0x14 → maxKill -0x10 → i -0xc）。
    int maxKill = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0 || m_killCount[i] != 0)
        {
            if (teamWin[i] == 1)
                killTeam1 += m_killCount[i];
            else if (teamWin[i] == 2)
                killTeam2 += m_killCount[i];
            else
                LogManager::logFormat(
                    1, "pvp.cpp",
                    __PRETTY_FUNCTION__,
                    0x157f,
                    "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                    i, (unsigned int)teamWin[i]);
        }
    }
    maxKill = killTeam1 > killTeam2 ? killTeam1 : killTeam2;
    // [本波·P56] ORIG 各 case 为 `cmpl $N,-0x10; jle <尾岛 mov $0>` + `mov $0x1,%eax`
    // 分支形（jle×4 + jmp×4）；直写 `return maxKill > N;` 被 4.4 折成 `setg` 单流 ⇒
    // 改 if-return 显式形。
    switch (teamCnt)
    {
    case 1:
        if (maxKill > 1)
            return true;
        return false;
    case 2:
        if (maxKill > 4)
            return true;
        return false;
    case 3:
        if (maxKill > 7)
            return true;
        return false;
    case 4:
        if (maxKill > 9)
            return true;
        return false;
    default:
        return true;
    }
