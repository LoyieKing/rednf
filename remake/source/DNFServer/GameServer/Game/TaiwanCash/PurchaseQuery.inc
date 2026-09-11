// ---- PurchaseQuery::PurchaseQuery —— ORIG 0x0817454c。----
// N76 封顶：uid 写入块 4.4 恒 LHS(this/ecx) 先物化、ORIG 4.1 恒 RHS(槽回读)先，
// 3 行时序差不可翻（exp/probe 见 GiftQuery 同判）。帧 0x28 已由 init 类内 inline 复原。
PurchaseQuery::PurchaseQuery(const char* account, const char* charac,
                             unsigned int price, unsigned int price2, long long uid)
{
    m_account = account;
    m_charac = charac;
    m_price = price;
    m_uid = uid;
    m_price2 = price2;
    m_resultCode = 0;
    m_trans.init();
}