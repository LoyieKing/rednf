bool BalanceQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_balance('%s', @out_balance, @out_result)",
                        m_account);
