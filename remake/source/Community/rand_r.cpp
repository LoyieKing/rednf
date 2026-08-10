//
// 反编译：恒返回 0（占位实现）。原始 timeGetTime 位于独立 TU（rand_r.cpp），
// 生成 _GLOBAL__I__Z11timeGetTimev 初始化桩。
//

#include "rand_r.h"

#include <cstddef>
#include "DNFFunctionLib.h"

unsigned int timeGetTime() {
    return 0;
}

// 原始：位于 rand_r.cpp TU（timeGetTime 之后），staticSeed 初值 0x05397fb1。
unsigned int Rand_r(unsigned int* seed) {
    static unsigned int staticSeed = 0x05397fb1;
    if (seed == NULL) {
        staticSeed += timeGetTime();
        return Rand_r(&staticSeed);
    }
    unsigned int x = *seed;
    x = x * 0x41c64e6d + 0x3039;
    unsigned int result = ((x >> 16) & 0x7ff) << 10;
    x = x * 0x41c64e6d + 0x3039;
    result = (result ^ (x >> 16 & 0x3ff)) << 10;
    x = x * 0x41c64e6d + 0x3039;
    result ^= (x >> 16 & 0x3ff);
    *seed = x;
    return result;
}
