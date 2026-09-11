// ---- confirm(const BillTransaction&) —— ORIG T 0x08173c48 ----
int TaiwanCash::confirm(const BillTransaction& trans)
{
    if (trans.m_tranNo == 0)   // [b28d-47] 同 cancel（ORIG long long 零比较形）
        return 0;

    ConfirmQuery query(trans);
    // ConfirmQuery: m_resultCode@+0x00
    if (!processBilling<ConfirmQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    cMyTrace(__FUNCTION__, 0xa7, 0)(   // [b28d-47] P227（同上）
        "[Taiwan, Billing] ConfirmQuery Transaction:%s",
        NumberToString(static_cast<unsigned long long>(trans.m_tranNo), 0));
    return 0;
}