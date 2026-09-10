unsigned long CMTRand::twist(const unsigned long& a, const unsigned long& b,
                             const unsigned long& c) const
{
    // [b28c-21] ORIG mov (%eax),%ebx 单步缓存 + 全程 %ebx 累计；纯 register
    // 关键字（非 register asm 绑定）令 4.4-O0 复现同形（probe1.cpp fA 逐条全等）。
    register unsigned long y = a;
    y ^= mixBits(b, c) >> 1;
    // ORIG neg+and+xor 无分支形：(0-t)&mask 复现；末步作为返回表达式直落 %eax
    // （写回 y 会多 mov %ebx,%eax）。
    return y ^ ((0UL - loBit(c)) & MT_MATRIX_A);
