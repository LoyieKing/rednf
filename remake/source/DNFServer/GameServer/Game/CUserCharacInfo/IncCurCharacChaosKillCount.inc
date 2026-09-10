void CUserCharacInfo::IncCurCharacChaosKillCount()
{
    enableSaveCharacStat();
    SetCurCharacChaosKillCount(GetCurCharacChaosKillCount() + 1);
