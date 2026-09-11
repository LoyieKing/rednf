void CUserCharacInfo::IncCurCharacChaosModeCount()
{
    enableSaveCharacStat();
    SetCurCharacChaosModeCount(GetCurCharacChaosModeCount() + 1);
}