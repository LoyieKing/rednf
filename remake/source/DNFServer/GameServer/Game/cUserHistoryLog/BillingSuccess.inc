void cUserHistoryLog::BillingSuccess(ENUM_BILLING_TYPE type,
                                     unsigned long long tranNo, int amount)
{
    // ORIG 0x086858a0：两分支各独立调 NumberToString（P48 去跨臂缓存，
    // `push %ebx`（第二臂 %ebx=amount）正由此双调用引入）。
    if (type == 3)
        m_user->LogHistory("BILLING, Success,%d,%s", (int)type,
                           NumberToString(tranNo, 0));
    else
        m_user->LogHistory("BILLING, Success,%d,%d,%d", (int)type,
                           NumberToString(tranNo, 0), amount);
