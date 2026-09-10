void CBillingRechargeCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        // [round5-GmCmdsA] 写侧用 union 的 int 视角（与 RHS(int) 同型 ⇒ 4.4 复现
        // ORIG 0x084b3ac8 的 LHS-first 直存；unsigned 视角会使 4.4 翻成 RHS-first）。
        m_amount = StringToNumber(m_parser->m_extra[2].c_str());
        return;                                    // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
