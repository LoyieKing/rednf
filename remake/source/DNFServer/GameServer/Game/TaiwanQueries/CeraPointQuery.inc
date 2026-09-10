CeraPointQuery::CeraPointQuery(const char* account, const char* p2,
                               char* uidStr, char* p4, unsigned int price,
                               char c6, char c7, char c8)
    : m_account(account), m_p2(p2), m_uidStr(uidStr), m_p4(p4),
      m_price(price), m_c6(c6), m_c7(c7), m_c8(c8), m_resultCode(0)
{
