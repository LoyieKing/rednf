// b28d-65 [N4/N102]：残余仅 m_traderCharac[0] 数组退化 add-vs-disp 1 条（同 TradeBegin
//   判词，七形探针封顶）；ORIG 0x0868507c，53=53。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
void cUserHistoryLog::TradeEnd(eTradeEndReason reason, int a, int b, int c, int d)
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (*(0, m_traderCharac) != 0) {
        static const int ILLEGAL_FACTOR = 10;   // ORIG _ZZN15cUserHistoryLog8TradeEndE15eTradeEndReasoniiiiE14ILLEGAL_FACTOR @0x8cf2690（=10）
        int sum1 = a + b;
        int sum2 = c + d;
        char flag = 0;
        if (sum1 * ILLEGAL_FACTOR < sum2)
            flag = 1;
        else if (sum2 * ILLEGAL_FACTOR < sum1)
            flag = 2;
        m_user->LogHistory("Trade-,\"%s\",\"%s\",%d, %d, %d, %d",
                           m_traderCharac, m_traderAccount, static_cast<int>(reason), sum1, sum2,
                           static_cast<int>(flag));
    }
}