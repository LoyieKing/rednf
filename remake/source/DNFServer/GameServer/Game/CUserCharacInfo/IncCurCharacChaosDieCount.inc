void CUserCharacInfo::IncCurCharacChaosDieCount()
{
    enableSaveCharacStat();
    SetCurCharacChaosDieCount(GetCurCharacChaosDieCount() + 1);
