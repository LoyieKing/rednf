bool BalancePointQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_balance_point('%s', @out_balance, @out_point, @out_result)",
        m_account);
