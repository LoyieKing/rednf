// ---- ceraPoint —— ORIG T 0x08173f0c ----
int TaiwanCash::ceraPoint(char* p1, char* p2, char command, char* p4, char* p5,
                          char p6, char p7, int price)
{
    // CeraPointQuery(p1, p2, p4, p5, (uint)price, p6, p7, command)；
    // m_resultCode@+0x18。
    CeraPointQuery query(p1, p2, p4, p5,
                         static_cast<unsigned int>(price),
                         p6, p7, command);
    if (!processBilling<CeraPointQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    {
        // [b28BM13] P227 全表达式临时链（原具名 trace 对象与 query 槽向相反，
        // N93/N82附；ORIG query@-0x34 trace@-0x18）。
        cMyTrace(__FUNCTION__, 0xe0, 0)(
            "[Taiwan, Billing] CeraPointQuery Command:%c, Price:%d",
            static_cast<int>(command), static_cast<int>(price));
    }
    return 0;
}