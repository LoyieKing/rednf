void CUser::OnRestrictTrade()
{
    // ORIG 0x08681312：与 OnPunishUser 同形（field 0x1d）。
    if (getCurCharacR() != 0)
    {
        // [b28d-solo-CUserOnlyOrig P0/P24] 同上：内联实参（ORIG 无 money 槽）。
        GetInstanceCirculationStatistic()->AddCirculationStatistic(
            (CIRCULATION_STATISTIC_FIELD)0x1d, this,
            getCurCharacInvenR()->get_money());
    }
