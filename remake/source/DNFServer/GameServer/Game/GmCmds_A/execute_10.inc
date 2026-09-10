void CBillingRechargeCommand::execute()
{
    CUser* user = GetUser();
    // ORIG（P157 逐点重取形）：每个调用表达式独立重读 s_pIPGHelper → 取 vptr →
    // add 槽 → deref fn，随后再读一次 helper 作 this；禁 vtable 缓存局部
    //（缓存少发 ORIG 的逐点 fetch 行且引入额外栈槽）。
    if (m_str0 == "cera")
    {
        // 读侧用 union 的 unsigned 视角（与 callee 形参 unsigned int 同型 ⇒ 单段直落）。
        GlobalData::s_pIPGHelper->IPGInput(user, 5, m_amountU, "", "", "GM", 0, 0, 0);
    }
    else if (m_str0 == "point")
    {
        GlobalData::s_pIPGHelper->IPGInputPoint(user, m_amountU, 4, 0, 0);
    }
    GlobalData::s_pIPGHelper->IPGQuery(user);
