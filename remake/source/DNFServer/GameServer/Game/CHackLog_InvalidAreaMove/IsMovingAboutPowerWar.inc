bool CHackLog_InvalidAreaMove::IsMovingAboutPowerWar(int a, int b)
{
    // ORIG 直连形：单表达式 return。
    return (IsHendonMyre(a) && IsWarfare(b)) ||
           (IsWarfare(a) && IsHendonMyre(b));
