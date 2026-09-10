bool DB_TOD_Random_Select_UserAPC::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    // ORIG 0x84400b8：sub $0xb0 空帧 + mov $1 + leave/ret（P31 未用局部压帧，
    // 实测 char[0xa4..0xb0] → sub $0xb0）。
    char buf[0xa8];
    (void)buf;
    return 1;
