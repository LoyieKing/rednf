bool CHackLog_InvalidAreaMove::IsMovingBetweenWestCoastAndGent(int a, int b, int c, int d)
{
    // ORIG 直连形：单表达式 return 不物化 0/1（同 IsMovingBetweenSeriaRoomAndGuildAgit）。
    return (IsGentEntranceInWestCoast(a, b) && IsWestCostEntranceInGent(c, d)) ||
           (IsWestCostEntranceInGent(a, b) && IsGentEntranceInWestCoast(c, d));
