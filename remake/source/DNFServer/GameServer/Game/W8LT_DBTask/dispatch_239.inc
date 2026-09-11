bool DB_TOD_Random_Select_UserAPC::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    // ORIG 0x84400b8：sub $0xb0 空帧 + mov $1 + leave/ret（P31 未用局部压帧，
    // 实测 char[0xa4..0xb0] → sub $0xb0）。
    char buf[0xa8];
    (void)buf;
    // [N8W8LT P139] ORIG nm 有 prett 局部串（0x08c5ba00, sz 0x47：`virtual bool
    // DB_TOD_Random_Select_UserAPC::dispatch(int, int, Stream*)`）而体无引用 ⇒ 原码有被
    // 丢弃的引用；sizeof 形只发射该串、零指令变化（探针实证）。
    (void)sizeof(__PRETTY_FUNCTION__);
    return 1;
}