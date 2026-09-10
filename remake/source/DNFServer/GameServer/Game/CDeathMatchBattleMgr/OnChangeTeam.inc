void CDeathMatchBattleMgr::OnChangeTeam(int team, int a)
{
    if (team < 0 || team > 7)
        return;
