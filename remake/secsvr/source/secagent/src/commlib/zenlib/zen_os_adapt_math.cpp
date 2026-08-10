// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_math.cpp
// Original quirks preserved: is_prime(0) and is_prime(1) are true, and
// is_prime(2) is false (even numbers are rejected before the trial loop).

#include <math.h>
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

bool is_prime(size_t n) {
    size_t test_max = (size_t)sqrt((double)n) + 1;
    size_t i;
    if (test_max <= 1) {
        return true;
    }
    if ((n & 1) == 0) {
        return false;
    }
    for (i = 3; i <= test_max; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

} // namespace ZEN_OS
