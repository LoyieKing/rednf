unsigned long CMTRand::randInt(const unsigned long& range)
{
    unsigned long mask = range;
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;

    unsigned long v;
    do
    {
        v = (unsigned long)randInt() & mask;
        } while (v > range);   // ORIG cmp/setb：v<=range 时退出
    return v;
}