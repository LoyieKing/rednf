int TaiwanCash::charge(char* p1, unsigned char serviceType, unsigned char p3,
                       unsigned int price, char* p5, char* p6, char* p7, char* p8,
                       unsigned long long uid, BillTransaction& trans)
{
    (void)p3;   // ORIG 未用于 ChargeQuery 构造——按汇编忠实
    (void)p5; (void)p6; (void)p7;
    (void)uid;  // 同上

    // ChargeQuery(p1, p8, "", price, (char)serviceType)；
    // m_resultCode@+0x14 / m_trans@+0x18。
    ChargeQuery query(p1, p8, "", price, static_cast<char>(serviceType));
    if (!processBilling<ChargeQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b27L8] ORIG 0x08173972 为 12B 结构整体赋值形（dest 缓存 + 3 连 mov，
    // BillTransaction 平凡 POD）；逐字段两行赋值出 dest 重载形（P195/P206 族）。
    trans = query.m_trans;

    {
        // [b28BM13 N96 清绑] ORIG 0x081739a3: N2S 返回经 %ebx 跨 cMyTrace ctor
        // 保活（mov %eax,%ebx → mov %ebx,0x14(%esp) 两步）。register 绑定违反
        // P0，改 P227 全表达式临时链形。
        cMyTrace(__FUNCTION__, 0xce, 0)(
            "[Taiwan, Billing] ChargeQuery Account:%s, Price:%d, Transaction:%s",
            p1, price, NumberToString(
                static_cast<unsigned long long>(trans.m_tranNo), 0));
    }
    return 0;
