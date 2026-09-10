int get_rand_int(int range) {
    if (range < 0) {
        return 0;
    }
    switch (range) {
    case 0:
        return rand();
    default:
        break;
    }
    int r = rand();
    if (r > range) {
        return rand() % range;
    }
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
    if (result > (unsigned int)range) {
        return result % range;
    }
    return result;
