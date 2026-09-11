int CObfuscator::Obfuscate2()                                // 0x08585be4
{
    // ORIG 槽序（P28）：idx(-0x1c 函数首 movl$0 预初始化，for 再赋 m_size-1)
    // → outIdx(-0x18) → tail(-0x14) → total8(-0x10 dead `shl$3;mov slot`) →
    // 复用字节槽(-0x9)：hi 装载/移位/lo 装载/移位全走同一 uchar 槽（4.1 单槽
    // 复用形，源码=同变量复合赋值 `hi <<= n; hi = ...; hi >>= n;`）。
    // 变量移位 ORIG 无 and $0x1f 掩码（4.1 直发 shl/sar %cl），去掩码。
    double framePad;                     // P31：ORIG 帧 0x34（4.1 esp 16B 对齐
                                         // 进位）；double 触发 4.4 帧 +4 且 int
                                         // 槽基整体下移，idx 落回 -0x1c
    (void)&framePad;
    int idx = 0;
    int outIdx = 0;
    int tail = 6;
    int total8 = m_size * 8;
    (void)total8;
    for (idx = m_size - 1; idx >= 0; --idx)
    {
        unsigned char hi = GetBit(idx, 0, tail);
        hi <<= (7 - tail);
        m_buffer[m_size - 1] = hi;
        hi = GetBit(idx, tail + 1, 7);
        hi >>= (tail + 1);
        m_buffer[m_size - 1] |= hi;
        m_buffer[outIdx] = m_buffer[m_size - 1];
        ++outIdx;
        if (tail == 0)
        {
            tail = 6;
        }
        else
        {
            --tail;
        }
    }
    return 0;
}