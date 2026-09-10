int TaiwanCash::purchase(unsigned long long uid, int price, int pay,
                         const char* account, const char* charac,
                         BillTransaction& trans, short kind)
{
    (void)kind;  // ORIG 该重载未使用 kind 参数

    PurchaseQuery query(account, charac,
                        static_cast<unsigned int>(price),
                        static_cast<unsigned int>(pay),
                        static_cast<long long>(uid));
    if (!processBilling<PurchaseQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x08173a00：12B 结构整体赋值 dest 缓存形（mov %edx,(%eax)
    // 三连）+ N2S 返回 %ebx 跨 ctor 保活（mov %eax,%ebx 两步）。
    trans = query.m_trans;

    // ORIG：NumberToString(trans.m_tranNo) + cMyTrace("[Taiwan, Billing] PurchaseQuery
    // Account:%s, Charac:%s, Price:%d, Transaction:%s")。此处保留等价 trace 面，
    // 经由 cMyTrace（GameTypes.h 局部实现）输出。【推断：等价日志】
    // ORIG 0x08173a00 直调 NumberToString(trans.m_tranNo,0)（禁 snprintf 替代）。
    {
        // [b28BM13 N96 清绑] register asm("ebx") 违反 P0；改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0x81, 0)(
            "[Taiwan, Billing] PurchaseQuery Account:%s, Charac:%s, Price:%d, Transaction:%s",
            account, charac, pay, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
