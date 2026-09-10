int CUser::getReturnUserLevelKey(int level, int mode)
{
    if (mode == 0)
    {
        if (level <= 9) return 5;
        if (level <= 0xe) return 10;
        if (level <= 0x13) return 0xf;
    }
    else
    {
        if (level <= 0x13) return 0xf;
    }
    if (level <= 0x1d) return 0x14;
    if (level <= 0x27) return 0x1e;
    if (level <= 0x31) return 0x28;
    if (level <= 0x3b) return 0x32;
    if (level <= 0x45) return 0x3c;
    return 0x46;
