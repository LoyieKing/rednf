GiftQuery::GiftQuery(const char* account, const char* sendCharac,
                     const char* recvCharac, unsigned int price,
                     unsigned int pay, long long uid)
{
    m_account = account;
    m_sendCharac = sendCharac;
    m_recvCharac = recvCharac;
    m_price = price;
    m_pay = pay;
    m_uid = uid;
    m_resultCode = 0;
    m_trans.init();
