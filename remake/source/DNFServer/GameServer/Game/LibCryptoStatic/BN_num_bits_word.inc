int BN_num_bits_word(unsigned long w)                // ORIG T 0x087fb680
{
    int r = 0;
    if (w & 0xffff0000u) {
        if (w & 0xff000000u) { r += 24; }
        else { r += 16; w >>= 16; }
    } else { /* 低半区 */ }
    w >>= r;                                         // ORIG shr 10/18 等价化
    return r + kBits8[w & 0xff];
}