void GetWorldMapCoordFromTable(int a, int b, STWorldmapPattern& out)
{
    // ORIG 0x088e5ef8：early-return 双出口（jle→nop+jmp / jle→nop）——
    // a>0 && b>0 时经 STWorldmapPattern::operator=（0x088f2e04）整块拷贝
    // table[(a-1)*400 + (b-1)*0x28]（[3][10] 行 stride 400B）；否则 out 不变。
    if (a <= 0)
        return;
    if (b <= 0)
        return;
    out = WorldMapScript::aWorldMapPatternTable_[a - 1][b - 1];
