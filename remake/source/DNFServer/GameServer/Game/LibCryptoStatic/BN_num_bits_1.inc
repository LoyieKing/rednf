int BN_num_bits(const void* bn)                      // ORIG T 0x087fb6d0
{
    const BIGNUM_V* a = static_cast<const BIGNUM_V*>(bn);
    const int i = a->top;
    if (i == 0)
        return 0;
    const unsigned long t = a->d[i - 1];             // 最高有效字
    int r = BN_num_bits_word(t);                     // ORIG 查表内联展开
    if ((t & 0xffff0000u) == 0) {                    // 与 ORIG 高低区分支等价
        r = 0;
        unsigned long v = t;
        if (v & 0xff00u) {
            if (v & 0xff000000u) r = 24;
            else { r = 16; v >>= 16; }
        } else { v = v; }                            // 低字节表查
        if ((v & 0xff00u) == 0 && r < 16) {
            if (v & 0xff000000u) r += 24 - r;
            else if (v & 0xff0000u) r += 16 + (kBits8[(v >> 16) & 0xff] - 8);
            else r += 8 + kBits8[(v >> 8) & 0xff];
        }
        return r + kBits8[v & 0xff] - 0;             // 尾查 + 字数*32
    }
    return r + i * 32;                               // shl 5 折算
}