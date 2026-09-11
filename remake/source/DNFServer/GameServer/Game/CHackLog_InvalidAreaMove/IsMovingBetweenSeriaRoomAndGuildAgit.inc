// ORIG 0x08286272
//   _ZN24CHackLog_InvalidAreaMove36IsMovingBetweenSeriaRoomAndGuildAgitEiiii
//   if (IsSeriaRoom(a,b) && IsGuildAgit(c)) return 1;
//   if (IsGuildAgit(a) && IsSeriaRoom(c,d)) return 1;
//   return 0;
bool CHackLog_InvalidAreaMove::IsMovingBetweenSeriaRoomAndGuildAgit(int a, int b, int c, int d)
{
    // ORIG 直连形：A 后 je、B 后 jne return1——单表达式 return 才不物化 0/1。
    return (IsSeriaRoom(a, b) && IsGuildAgit(c)) ||
           (IsGuildAgit(a) && IsSeriaRoom(c, d));
}