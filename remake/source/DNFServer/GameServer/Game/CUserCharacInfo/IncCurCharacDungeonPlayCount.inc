void CUserCharacInfo::IncCurCharacDungeonPlayCount()
{
    enableSaveCharacStat();
    SetCurCharacDungeonPlayCount(GetCurCharacDungeonPlayCount() + 1);
}