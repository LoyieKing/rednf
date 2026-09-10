bool CHackLog_InvalidAreaMove::IsInvalidMovableArea(int a, int b, int c, int d)
{
    // ORIG xor 链形：谓词 bool 声明 + 全链取反 if（call;xor $0x1;test;je 连锁 + 尾物化块）。
    if (!IsMovingBetweenWestCoastAndGent(a, b, c, d) &&
        !IsMovingAboutPowerWar(a, c) &&
        !IsMovingInGuildAgit(a, c) &&
        !IsExceptionMove(a, b, c, d) &&
        !IsMovingBetweenSeriaRoomAndGuildAgit(a, b, c, d))
        return 1;
    return 0;
