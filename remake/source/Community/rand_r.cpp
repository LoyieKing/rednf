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
        // ORIG 4.4.6-3 代码生成：call 结果先 mov %eax,%edx，再装载 staticSeed，
        // 再 lea (%edx,%eax,1),%eax；必须用 (int) 转换包裹使树形不匹配 RMW 折叠。
        staticSeed = (unsigned int)((int)staticSeed + (int)timeGetTime());
        return Rand_r(&staticSeed);
    }
    unsigned int x = *seed;
    x = x * 0x41c64e6d;
    x += 0x3039;
    unsigned int result = (x >> 16) & 0x7ff;
    x = x * 0x41c64e6d;
    x += 0x3039;
    result <<= 10;
    // ORIG 的 xor 为寄存器形态（mov %eax,%edx; and; mov result,%eax; xor; mov），
    // 直接 ^= 会被 -O0 折叠为内存 RMW（xor %eax,-0xc(%ebp)）；(int) 转换包裹可复现。
    result = (unsigned int)((int)result ^ (int)((x >> 16) & 0x3ff));
    x = x * 0x41c64e6d;
    x += 0x3039;
    result <<= 10;
    result = (unsigned int)((int)result ^ (int)((x >> 16) & 0x3ff));
    *seed = x;
    return result;
}
