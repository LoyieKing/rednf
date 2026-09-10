int CObfuscator::Deobfuscate1()                              // 0x08585aaa
{
    // 同 Obfuscate1：i 提首 + total 槽物化（ORIG 0x08585aaa 头同构）。
    int i = 0;
    int total = m_size * 8;
    int cnt = total / 7;
    m_field0c = total % 7;
    for (i = 0; i <= cnt; ++i)
    {
        if ((m_field0c == 0) && (i == cnt))
        {
            break;                                            // ORIG 循环条件排除该次迭代
        }
        unsigned char bits = 0;                               // ORIG `movb $0,-0x9`
        if ((i == 0) && (m_field0c != 0))
        {
            bits = GetBitFromLSB(m_field0c);
        }
        else
        {
            bits = GetBitFromLSB(7);
        }
        if (i > 0)
        {
            ShiftLeft(7, m_buffer);
        }
        m_buffer[m_size - 1] |= bits;
    }
    return 0;
