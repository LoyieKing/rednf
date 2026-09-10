bool WindowsDataFileFamily::GenerateRange(unsigned long* outStart,
                                          unsigned long* outLen)   // 0x08575230
{
    // ORIG 槽向 span@-0x10 / clipped@-0x9；4.4 先声明占深槽（N93 标量版，
    // 同 ncl_check_digit_str 复现），span 未初始化前置不改指令序。
    unsigned long span;
    bool clipped = false;   // ORIG prologue 后即预清 -0x9（P198①：声明/预清在 IsEmpty 前）
    if (m_files.empty())
    {
        return false;
    }
    span =
        static_cast<unsigned long>(m_randMin) +
        static_cast<unsigned long>(rand()) %
            (static_cast<unsigned long>(m_randMax) + 1U);

    // ORIG: clip+置位同在单一 if 块（ja 直跳输出区）；拆两 if 会被 GCC 合并出错序中间测试。
    if (m_maxSize <= m_scanOffset + span)
    {
        span = m_maxSize - m_scanOffset;
        clipped = true;
    }
    *outStart = m_scanOffset;
    *outLen = span;
    if (clipped)
    {
        m_scanOffset = 0;                                    // 抵尾复位游标
    }
    else
    {
        m_scanOffset += span;
    }
    return true;
