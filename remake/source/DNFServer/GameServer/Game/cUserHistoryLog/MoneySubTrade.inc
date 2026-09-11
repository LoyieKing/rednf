// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
void cUserHistoryLog::MoneySubTrade(int money, int sub, eMoneySubReason reason)
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (*(0, m_traderCharac) != 0)
        m_user->LogHistory("Money-,%d,%d,%d,%s(%s)", money, sub, static_cast<int>(reason),
                           m_traderCharac, m_traderAccount);
}