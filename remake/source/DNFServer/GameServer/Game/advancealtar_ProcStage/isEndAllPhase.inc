bool ProcStage::isEndAllPhase() const
{
    // b28c-L5：同 isEndSummon —— ORIG 0x0812eb5a `mov 0xc(%eax),%ebx` 一步直落
    //   （无 static_cast，隐式 usual conversion 保 `cmp %eax,%ebx / setae`）+
    //   尾 if-return 物化岛（P56）；现形 cast 致两段中转、直返省整段岛（少 4 条）。
    if (m_timeLine == 0)
    {
        return false;
    }
    if (m_fieldc >= m_timeLine->m_phases.size())
    {
        return true;
    }
    return false;
