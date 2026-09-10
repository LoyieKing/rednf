void CUserCharacInfo::IncCurCharacLuckPoint(int point)
{
    enableSaveCharacStat();
    if (GetCurCharacLuckPoint() == 100000)
        return;
    SetCurCharacLuckPoint(GetCurCharacLuckPoint() + point);
