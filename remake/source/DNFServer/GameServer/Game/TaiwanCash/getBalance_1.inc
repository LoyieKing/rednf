int TaiwanCash::getBalance(const char* account, int& balance, int& point)
{
    // BalancePointQuery: m_balance@+0x04 / m_point@+0x08 / m_resultCode@+0x0c
    BalancePointQuery query(account);
    if (!processBilling<BalancePointQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    balance = query.m_balance;
    point = query.m_point;
    cMyTrace(__FUNCTION__, 0x5a, 0)(   // [b28d-47] P227（同上）
        "[Taiwan, Billing] BalancePointQuery Balance:%d, Point:%d, Result:%d",
        balance, point, query.m_resultCode);
    return 0;
