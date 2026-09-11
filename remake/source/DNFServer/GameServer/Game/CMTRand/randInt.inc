int CMTRand::randInt()
{
    if (m_left == 0)
    {
        reload();
    }

    --m_left;

    // [b28c-21] ORIG mov (%eax),%ebx 跨段缓存；纯 register 关键字复现（P0 限定：
    // 仅 register asm("reg") 绑定禁用；纯 register 为标准 C++ 存储建议）。
    register unsigned long y = *m_next++;
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    // 末步不写回 y：ORIG xor %ebx,%eax 直落返回值。
    return (int)(y ^ (y >> 18));
}