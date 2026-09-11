void CUserCharacInfo::IncCurCharacAssaultCount()
{
    enableSaveCharacStat();
    SetAssaultCount(GetAssaultCount() + 1);
}