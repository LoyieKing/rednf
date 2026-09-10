bool CUserCharacInfo::IsAvailableCurCharacTradeGoldDaily(unsigned int gold)
{
    if (!m_selected)
        return false;
    unsigned int limit = get_charac_level() * get_charac_level() * 10000;
    if (limit < CUR->m_reliablePerson.GetTradeGoldDaily() + gold)
    {
        // [b28d-solo-CUserCharacInfo P227/P37] ORIG 0x0864657e：varargs 先求值（右起）
        // 且 GetTradeGoldDaily 值经 %edi 跨 ctor 保活（push edi）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x3f1, 0)(
            "TRADE_RESTIRCTION charac_name(%s), charac_level(%d), limit_gold(%u), "
            "add_gold(%u), use_gold(%u)",
            getCurCharacName(), get_charac_level(), limit, gold,
            CUR->m_reliablePerson.GetTradeGoldDaily());
        return false;
    }
    return true;
