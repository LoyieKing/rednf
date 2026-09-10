bool DB_LoadPowerWarStatueRanker::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    // ORIG 0x8434878：sub $0x40 空帧 + mov $1 + leave/ret（P31，
    // 实测 char[0x34..0x40] → sub $0x40）。
    char buf[0x38];
    (void)buf;
    return 1;
