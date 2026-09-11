int CObfuscator::Deobfuscate2()                              // 0x08585d2e
{
    // 与 Obfuscate2 同构（同 double-pad 帧形 + 单 uchar 槽复合赋值链）；
    // tail 初值 = m_size%7，若 0 → 6 否则 --（ORIG +0x060 cmpl$0/jne 形）。
    double framePad;                     // P31 帧对齐 pad（同 Obfuscate2）
    (void)&framePad;
    int idx = 0;
    int outIdx = 0;
    int tail = 0;                          // ORIG 函数首 movl$0,-0x14 预初始化
    int total8 = m_size * 8;
    (void)total8;
    tail = m_size % 7;
    if (tail == 0)
    {
        tail = 6;
    }
    else
    {
        --tail;
    }
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