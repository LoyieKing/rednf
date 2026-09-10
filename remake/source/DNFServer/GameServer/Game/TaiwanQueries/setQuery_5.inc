bool CancelQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_cancel(%s, @out_code)",
                        NumberToString(m_trans.m_tranNo, 0));
