// Restored from zergsvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_os_adapt_math.cpp
// zergsvr 相对 gunnersvr：gcd 逐字节一致；is_prime 改为“区间内找最小因子”：
//   is_prime(n, start, end) 返回 [start, end] 中能整除 n 的最小值，找不到返回 0，
//   n <= 3 或 start > end 直接返回 0；
//   is_prime(n) = is_prime(n, 2, n >> 1)。

#include <stddef.h>
#include <stdint.h>

namespace ZEN_OS {

uint32_t gcd(uint32_t x, uint32_t y) {
    unsigned long r;
    while (y != 0) {
        r = x % y;
        x = y;
        if (r == 0) {
            return y;
        }
        y = r;
    }
    return x;
}

uint32_t is_prime(uint32_t n, uint32_t start, uint32_t end) {
    uint32_t i = start;
    if (n <= 3 || start > end) {
        return 0;
    }
    if ((n / i) * i == n) {
        return i;
    }
    for (++i; i <= end; ++i) {
        if ((n / i) * i == n) {
            return i;
        }
    }
    return 0;
}

uint32_t is_prime(uint32_t n) {
    return is_prime(n, 2, n >> 1);
}

} // namespace ZEN_OS
