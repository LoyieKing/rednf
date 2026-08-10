// df_statics_r — TaskImpl（ORIG TaskImpl.o 拆分）
#include <stdlib.h>



unsigned int get_rand_int(int range)
{
    if (range < 0)
    {
        return 0;
    }
    if (range == 0)
    {
        return rand();
    }
    int r = rand();
    if (range < r)
    {
        r = rand();
        return (unsigned int)(r % range);
    }
    r = r * 0x41c64e6d + 0x3039;
    int r1 = r * 0x41c64e6d + 0x3039;
    int r2 = r1 * 0x41c64e6d + 0x3039;
    unsigned int v = ((((r >> 16) & 0x7ff) << 10) ^ ((r1 >> 16) & 0x3ff)) << 10 ^
                     ((r2 >> 16) & 0x3ff);
    if ((unsigned int)range < v)
    {
        v = v % (unsigned int)range;
    }
    return v;
}
