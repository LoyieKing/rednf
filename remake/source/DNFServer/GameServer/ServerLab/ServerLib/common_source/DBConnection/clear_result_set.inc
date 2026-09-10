void DBConnection::clear_result_set()
{
    if (m_res != (MYSQL_RES*)0)
    {
        mysql_free_result(m_res);
    }
    m_res = (MYSQL_RES*)0;
    m_row = (MYSQL_ROW)0;
