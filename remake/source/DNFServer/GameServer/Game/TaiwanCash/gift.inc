int TaiwanCash::gift(unsigned long long uid, int price, int pay,
                     const char* account, const char* sendCharac,
                     const char* recvCharac, const char* giftMsg,
                     BillTransaction& trans)
{
    (void)giftMsg;  // ORIG 未把该参传入 GiftQuery——忠实转写


    // GiftQuery(account, sendCharac, recvCharac, uint(price=p2), uint(pay=p3), uid)；
    // m_resultCode@+0x1c / m_trans@+0x20（asm: esp+0x10=*(ebp+0x14)、esp+0x14=*(ebp+0x18)）。
    GiftQuery query(account, sendCharac, recvCharac,
                    static_cast<unsigned int>(price),
                    static_cast<unsigned int>(pay),
                    static_cast<long long>(uid));
    if (!processBilling<GiftQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x08173b20：12B 结构整体赋值 dest 缓存形 + NumberToString
    // 结果跨 ctor %ebx 保活。
    trans = query.m_trans;

    // ORIG +0x0bf 直调 NumberToString(m_tranNo,0)（禁 snprintf 替代，P167 判据）。
    {
        // [b28BM13 N96 清绑] register asm("ebx") 违反 P0；改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0x95, 0)(
            "[Taiwan, Billing] GiftQuery Account:%s, SendCharac:%s, RecvCharac:%s, Price:%d, Transaction:%s",
            account, sendCharac, recvCharac, pay, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
