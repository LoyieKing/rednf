int CObfuscator::Obfuscate1()                                // 0x08585986
{
    // ORIG 槽序（P28/P38）：i(-0x18 函数首 `movl $0,-0x18` 预初始化) →
    // total(-0x14，`shl$3; mov slot` 一次物化，/7 与 %7 两条 magic 链均
    // `mov -0x14,%ecx` reload) → cnt(-0x10)。
    int i = 0;
    int total = m_size * 8;
    int cnt = total / 7;
    m_field0c = total % 7;                                   // 余位（this+0xc）
    for (i = 0; i <= cnt; ++i)
    {
        unsigned char bits = GetBitFromLSB(7);
        if ((i >= 1) && (i < cnt))
        {
            ShiftLeft(7, m_buffer);
        }
        else if (i == cnt)
        {
            ShiftLeft(m_field0c, m_buffer);
        }
        m_buffer[m_size - 1] |= bits;
    }
    return 0;
}