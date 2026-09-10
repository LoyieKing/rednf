int TaiwanCash::purchase(unsigned long long uid, int price, int pay,
                         const char* account, const char* charac,
                         BillTransaction& trans)
{
    PurchaseQuery query(account, charac,
                        static_cast<unsigned int>(price),
                        static_cast<unsigned int>(pay),
                        static_cast<long long>(uid));
    if (!processBilling<PurchaseQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x081738e6：12B 结构整体赋值 dest 缓存形 + N2S %ebx 保活。
    trans = query.m_trans;

    {
        // [b28BM13 N96 清绑] register asm("ebx") 违反 P0；改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0x6e, 0)(
            "[Taiwan, Billing] PurchaseQuery Account:%s, Charac:%s, Price:%d, Transaction:%s",
            account, charac, pay, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
