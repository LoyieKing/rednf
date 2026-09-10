void stQuestTriggerState_t::subTriggerState(char v)
{
    if (v == 0)
    {
        // ORIG 0x086ad6fc：`mov (%eax),%eax; lea -1(%eax),%edx; mov %edx,(%eax)` ——
        // 对 +0x00 的 4 字节视图（m_trigger0/m_trigger1 同容器）裸自减 1（m_low32）。
        --m_low32;
        return;
    }
    // ORIG 自减段：读同 inc（`add $0x1ff,%ax` 即 16 位 -1）+ 位域写回保留高 7 位。
    if (v & 0x10)
    {
        if (m_trigger0 > 0)
            --m_trigger0;
    }
    if (v & 0x20)
    {
        if (m_trigger1 > 0)
            --m_trigger1;
    }
    if (v & 0x40)
    {
        if (m_trigger2 > 0)
            --m_trigger2;
    }
