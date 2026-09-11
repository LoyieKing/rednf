void GetWorldMapCoordFromTable2ND(int a, STWorldmapPattern2ND& out)
{
    // ORIG 0x088e5f44：js→nop 出口 + body 后 jmp→pop（if 块 + 显式 return）——
    // a>=0 时逐字段复制 table2ND[a]（7 int，0x1c 字节，隐式 operator= 展开无 call）。
    if (a >= 0)
        out = WorldMapScript::aWorldMapPatternTable2ND_[a];
    return;
}