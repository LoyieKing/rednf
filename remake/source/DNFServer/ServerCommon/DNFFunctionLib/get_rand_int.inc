unsigned int DNFFLib::get_rand_int(int divide) {
    if (divide < 0) {
        return 0;
    }
    switch (divide) {
    case 0:
        return rand();
    default:
        break;
    }
    int r = rand();
    // 原始：r > range 时直接返回 rand() % range
    if (r > divide) {
        return rand() % divide;
    }
    // 原始：单累加器逐步 LCG（imul → 存栈 → add [ebp]；result 逐步 shl/xor）
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int result = (r / 65536) & 0x7ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    // 原始：result > range（无符号）才取模
    if (result > (unsigned int)divide) {
        return result % divide;
    }
    return result;
