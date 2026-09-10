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
