// ---- getBalance(const char*, int&) —— ORIG T 0x08173766 ----
int TaiwanCash::getBalance(const char* account, int& balance)
{
    // BalanceQuery: m_balance@+0x04 / m_resultCode@+0x08
    BalanceQuery query(account);
    if (!processBilling<BalanceQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    balance = query.m_balance;
    // [b28d-47] P227 全表达式临时链：ORIG 0x081737c6 `mov -0x1c(%ebp),%esi;
    // mov (%eax),%ebx` 在 ctor 前物化两实参并跨 ctor 保活。
    cMyTrace(__FUNCTION__, 0x46, 0)(
        "[Taiwan, Billing] BalanceQuery Balance:%d, Result:%d",
        balance, query.m_resultCode);
    return 0;
}