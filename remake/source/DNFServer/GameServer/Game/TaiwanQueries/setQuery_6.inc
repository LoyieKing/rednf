bool ChargeQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_recharge('%s', '%s', '%s', %u, %d, @out_tran, @out_code)",
        m_account, m_p2, m_p3, m_price, static_cast<int>(m_chargeType));
