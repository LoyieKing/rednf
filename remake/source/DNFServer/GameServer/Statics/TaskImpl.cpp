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
    if (r > range)
    {
        return (unsigned int)(rand() % range);
    }
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int v = (r / 65536) & 0x7ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    v <<= 10;
    v ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    v <<= 10;
    v ^= (r / 65536) & 0x3ff;
    if ((unsigned int)range < v)
    {
        return v % (unsigned int)range;
    }
    return v;
}
