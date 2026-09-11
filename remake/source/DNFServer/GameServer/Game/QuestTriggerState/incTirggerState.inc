void stQuestTriggerState_t::incTirggerState(char v)
{
    // ORIG 0x086ad602：读 `movzwl; andw $0x1ff,%ax; movzwl %ax,%eax; cmp $0x3fe`，
    // 自增段 `movzwl; andw $0x1ff,%ax; add $0x1,%eax; andw $0x1ff,%ax` + 位域写回
    // （保留高 7 位）。位域 `++m_triggerN` 逐条复现（探针 /tmp/qts/b.cpp，73=73）。
    if (v & 0x10)
    {
        if (m_trigger0 <= 0x3fe)
            ++m_trigger0;
    }
    if (v & 0x20)
    {
        if (m_trigger1 <= 0x3fe)
            ++m_trigger1;
    }
    if (v & 0x40)
    {
        if (m_trigger2 <= 0x3fe)
            ++m_trigger2;
    }
}