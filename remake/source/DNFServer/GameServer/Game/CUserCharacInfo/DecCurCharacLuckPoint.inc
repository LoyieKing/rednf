void CUserCharacInfo::DecCurCharacLuckPoint(int percent)
{
    enableSaveCharacStat();
    SetCurCharacLuckPoint(GetCurCharacLuckPoint() - GetTenThousandPercentage(GetCurCharacLuckPoint(), percent));
}