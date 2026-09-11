void CDungeonClear::fromString(CUser* pUser, char* str)
{
    // ORIG 帧 0x38（仅一次 12 字节出参调用，其余为死局部）：P31 dummy local
    // 撑帧对齐（ORIG 0x084843fa，ndiff=2 残余即 sub $0x38 vs $0x18）。
    char framePad[0x20];
    (void)framePad;
    AradfromString(pUser, str);
}