void CAssaultPlace::TimeoutCloseAssault()
{
    // b28d-solo-CAssaultMgr: **IDENTICAL_AE（清单 DIFF 64 → AE）** — 唯一残差
    // 为 ORIG 082ea21a `mov 0x10c(%eax),%eax; mov %eax,%ebx`（m_state 载入后
    // eax→ebx 中转）vs 4.4 恒折 `mov 0x10c(%eax),%ebx`。修法：trace 实参
    // `(unsigned int)m_state` —— 探针 /tmp/probe_trace.cpp 三形实测：int 字段
    // 单段直落 %ebx，enum 字段与 unsigned cast 均产两段（`mov mem,%eax;
    // mov %eax,%ebx`=ORIG 形）⇒ 异型转换中转复现（P254/P265 同族）。
    // b28d-solo-CAssaultMgr: ORIG 0x082ea188 先把 8 个 GetCharacName 实参求值
    // （-0x40..-0x2c 槽 + edi/esi），**之后**才 ctor cMyTrace（+0x30 段）⇒ 原码是
    // 全表达式临时链（P227/P37），具名对象恒 ctor 先于实参求值（DIFF 主因）。
    cMyTrace(__PRETTY_FUNCTION__,
             0x9ea, 0)("TimeoutCloseAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
          (unsigned int)m_state,
          m_assaulters[0].GetCharacName(), m_assaulters[1].GetCharacName(),
          m_assaulters[2].GetCharacName(), m_assaulters[3].GetCharacName(),
          m_assaulters[4].GetCharacName(), m_assaulters[5].GetCharacName(),
          m_assaulters[6].GetCharacName(), m_assaulters[7].GetCharacName());

    // b28d-solo-CAssaultMgr: ORIG 尾块 `call CloseAssault` 后直落 epilogue；
    // 早退 return 使 4.4 尾块补 `jmp <epilogue>; nop`（+2）。改单出口正向包裹
    // （`==5 || ==6` 短路链）复现 ORIG `cmp $5;je body; cmp $6;jne epilogue`。
    if (m_state == 5 || m_state == 6)
    {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
        m_state = (ENUM_ASSAULT_STATE)7;
        CloseAssault();
    }
