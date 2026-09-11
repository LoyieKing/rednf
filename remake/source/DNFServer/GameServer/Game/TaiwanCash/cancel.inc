// ---- cancel(const BillTransaction&) —— ORIG T 0x08173d1c ----
int TaiwanCash::cancel(const BillTransaction& trans)
{
    // [b28d-47] ORIG 0x08173d24 `mov 0x8(%eax),%edx; mov 0x4(%eax),%eax;
    // or %edx,%eax; test` = 直接 `m_tranNo == 0`（long long 零比较）；旧 lo|hi
    // 写法多两次 64 位读取 + `mov %edx,%eax; xor %edx,%edx` 移位链。
    if (trans.m_tranNo == 0)
        return 0;

    CancelQuery query(trans);
    // CancelQuery: m_resultCode@+0x00
    if (!processBilling<CancelQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b28d-47] P227 全表达式临时链（ORIG 0x08173xxx 在 ctor 前物化 NumberToString
    // 结果并跨 ctor 以 callee-saved 保活；具名 tranStr/trace 会落槽 + 帧漂移）。
    cMyTrace(__FUNCTION__, 0xb9, 0)(
        "[Taiwan, Billing] CancelQuery Transaction:%s",
        NumberToString(static_cast<unsigned long long>(trans.m_tranNo), 0));
    return 0;
}