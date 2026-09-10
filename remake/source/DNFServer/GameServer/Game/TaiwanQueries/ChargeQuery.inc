ChargeQuery::ChargeQuery(const char* account, const char* p2, const char* p3,
                         unsigned int price, char chargeType)
    : m_account(account), m_p2(p2), m_p3(p3), m_price(price),
      m_chargeType(chargeType)
{
    m_trans.init();
    // [b27L8] ORIG 0x08174efe：m_resultCode=0（movl $0x0,0x14）在 m_trans.init
    // call 之后发射；初始化列表形会提前到 call 前。
    m_resultCode = 0;
