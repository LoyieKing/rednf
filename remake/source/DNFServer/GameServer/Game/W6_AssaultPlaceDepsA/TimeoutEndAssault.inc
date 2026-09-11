// ---------------------------------------------------------------------------
// ORIG T 0x82ea004 (size 0x183)：TimeoutEndAssault()
// state==4 才推进到 5 并 EndAssault。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（P321-T5）：P227 全表达式临时链复现
//   ctor 前右起 varargs 求值 + P285 正向包裹消 epilogue 尾岛后，仅剩 N18a——ORIG
//   0x082ea099 `mov 0x10c(%eax),%eax; mov %eax,%ebx` 两段 regmove 被 4.4 折单段
//   （`mov 0x10c(%eax),%ebx`）。S1 一次替代形 `(ENUM_ASSAULT_STATE)(unsigned int)`
//   实测仍单段（已恢复直证形）；m_state int→enum 属 .h 跨 TU 改动（S2 归 Main）。
//   p167 CLEAR。ORIG 0x082ea004
void CAssaultPlace::TimeoutEndAssault()
{
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ea00d 段：8 个 GetCharacName 在
    //   cMyTrace ctor call **之前**右起求值（[7]→[0]，前 6 个落槽 -0x40..-0x2c、
    //   后 2 个落 %edi/%esi、state 落 %ebx），ctor 后才压 varargs —— P227 全表达式
    //   临时链形（具名局部 trace 使 4.4 把 ctor 提前，帧 0x5c vs ORIG 0x6c）。
    cMyTrace(__PRETTY_FUNCTION__, 0x9ce, 0)(
        "TimeoutEndAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
        // P265：ORIG 0x082ea099 `mov 0x10c(%eax),%eax; mov %eax,%ebx` 两段中转
        //   ⇒ 实参处显式 enum cast（int 字段 + (ENUM_X) 实参）。
        // [b28d-solo-W6_AssaultPlaceDepsA] S1 已试 `(ENUM_ASSAULT_STATE)(unsigned int)`
        //   替代形——实测仍 `mov 0x10c(%eax),%ebx` 单段（无效，已恢复直证形态）⇒
        //   残差仅 1 条 regmove（T5）封 N。
        (ENUM_ASSAULT_STATE)m_state,
        m_assaulters[0].GetCharacName(), m_assaulters[1].GetCharacName(),
        m_assaulters[2].GetCharacName(), m_assaulters[3].GetCharacName(),
        m_assaulters[4].GetCharacName(), m_assaulters[5].GetCharacName(),
        m_assaulters[6].GetCharacName(), m_assaulters[7].GetCharacName());

    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ea119 `jne +0x17b`（落 epilogue）
    //   + 体尾直落 ⇒ 正向包裹（早返形使 4.4 多 `jmp <epilogue>; nop` 尾岛，N121）。
    if (m_state == 4) {
        m_state = (ENUM_ASSAULT_STATE)5;
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x05');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x05');
        EndAssault();
    }
}