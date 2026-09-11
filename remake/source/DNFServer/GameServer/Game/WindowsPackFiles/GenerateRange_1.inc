bool WindowsExeFileFamily::GenerateRange(long* outSecIdx,
                                         unsigned long* outStart,
                                         unsigned long* outLen)     // 0x08574b8e
{
    // [b28d-solo-WindowsPackFiles] ORIG 槽布局（0x08574b95 起）：clipped@-0x9
    // （prologue 后立即 movb $0x0 预清）、section@-0x10、span@-0x14。4.4 探针实测：
    // 声明序 `span; const Interval* section; bool clipped=false;` + 指针赋值形
    // 精确复现该三槽（4 形探针 /tmp/wpf_probe/p.cpp genD，其余三形 -0x11/-0xd）。
    // 条件本体 ORIG 为 `cmp %eax,%edx; ja +0xae` 直跳（非 setbe 物化）⇒ if 直判。
    unsigned long span;
    const Interval* section;
    bool clipped = false;
    if (m_intervals.empty())
    {
        return false;
    }
    span =
        static_cast<unsigned long>(m_randMin) +
        static_cast<unsigned long>(rand()) %
            (static_cast<unsigned long>(m_randMax) + 1U);

    section = &m_intervals[m_curSectionIndex];
    if (static_cast<unsigned long>(section->m_start + section->m_size) <=
        m_curOffset + span)
    {
        span = static_cast<unsigned long>(section->m_start + section->m_size) -
               m_curOffset;
        clipped = true;
    }
    *outSecIdx = m_curSectionIndex;
    *outStart = m_curOffset;
    *outLen = span;
    if (clipped)
    {
        m_curSectionIndex += 1;
        // [b28d-solo-WindowsPackFiles] ORIG 0x08574c88 比较方向为
        // `call size; mov idx,%edx; cmp %edx,%eax; setbe`（size() <= idx，
        // call 后重读成员）；写 `idx >= size()` 会引 %ebx 跨 call 缓存 + setae。
        if (m_intervals.size() <= static_cast<unsigned long>(m_curSectionIndex))
        {
            m_curSectionIndex = 0;
        }
        m_curOffset = m_intervals[m_curSectionIndex].m_start;
    }
    else
    {
        m_curOffset += span;
    }
    return true;
}