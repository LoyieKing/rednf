// ORIG 0x08286222
//   _ZN24CHackLog_InvalidAreaMove21IsMovingAboutPowerWarEii
//   if (IsHendonMyre(a) && IsWarfare(b)) return 1;
//   if (IsWarfare(a) && IsHendonMyre(b)) return 1;
//   return 0;
bool CHackLog_InvalidAreaMove::IsMovingAboutPowerWar(int a, int b)
{
    // ORIG 直连形：单表达式 return。
    return (IsHendonMyre(a) && IsWarfare(b)) ||
           (IsWarfare(a) && IsHendonMyre(b));
}