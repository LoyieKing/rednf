bool PurchaseQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_purchase('%s', '%s', %u, %u, @out_tran, @out_code)",
                        m_account, m_charac, m_price, m_price2);
