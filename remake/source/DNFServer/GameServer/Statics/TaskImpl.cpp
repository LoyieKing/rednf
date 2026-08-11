// df_statics_r — TaskImpl（ORIG TaskImpl.o 拆分）
#include <stdlib.h>



// ORIG 由无 CMOV 目标（i586 形态）编译：除法/比较展开为 sar/shr/lea 而非
// lea/cmovs，须函数级 target 覆盖（§89）；range==0 分支 ORIG 为 switch 单 case
// 形态（mov+test 而非 cmpl），与 DNFFLib::get_rand_int 同构。
__attribute__((target("arch=i586")))
unsigned int get_rand_int(int range)
{
    if (range < 0)
    {
        return 0;
    }
    switch (range)
    {
    case 0:
        return rand();
    default:
        break;
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
