long GetSlicedInterval(long* out1, long* out2, long* out3, long* out4,
                       long total, long firstLen, long step)
{
    long count = 0;
    if (firstLen > 0)
    {
        *out1 = 0;
        *out2 = firstLen;
        // b28c-l23：ORIG 此处为 `addl $0x1,-0x4(%ebp)`（自增，非赋值）。
        ++count;
    }
    if (firstLen + step < total)
    {
        if (count == 0)
        {
            *out1 = firstLen + step;
            *out2 = total - (firstLen + step);
        }
        else
        {
            *out3 = firstLen + step;
            *out4 = total - (firstLen + step);
        }
        count += 1;
    }
    return count;
