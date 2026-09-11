// ORIG 0x082861b6
//   _ZN24CHackLog_InvalidAreaMove31IsMovingBetweenWestCoastAndGentEiiii
//   if (IsGentEntranceInWestCoast(a,b) && IsWestCostEntranceInGent(c,d)) return 1;
//   if (IsWestCostEntranceInGent(a,b) && IsGentEntranceInWestCoast(c,d)) return 1;
//   return 0;
bool CHackLog_InvalidAreaMove::IsMovingBetweenWestCoastAndGent(int a, int b, int c, int d)
{
    // ORIG 直连形：单表达式 return 不物化 0/1（同 IsMovingBetweenSeriaRoomAndGuildAgit）。
    return (IsGentEntranceInWestCoast(a, b) && IsWestCostEntranceInGent(c, d)) ||
           (IsWestCostEntranceInGent(a, b) && IsGentEntranceInWestCoast(c, d));
}