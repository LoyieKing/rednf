bool CHackLog_InvalidAreaMove::IsMovingBetweenSeriaRoomAndGuildAgit(int a, int b, int c, int d)
{
    // ORIG 直连形：A 后 je、B 后 jne return1——单表达式 return 才不物化 0/1。
    return (IsSeriaRoom(a, b) && IsGuildAgit(c)) ||
           (IsGuildAgit(a) && IsSeriaRoom(c, d));
