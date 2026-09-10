bool GiftQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_gift('%s', '%s', '%s', %u, %u, @out_tran, @out_code)",
        m_account, m_sendCharac, m_recvCharac, m_price, m_pay);
