bool DBConnection::fetch()
{
    if (m_res == (MYSQL_RES*)0)
    {
        return false;
    }
    m_row = (MYSQL_ROW)mysql_fetch_row(m_res);
    if (m_row != (MYSQL_ROW)0)
    {
        m_len = (ulong*)mysql_fetch_lengths(m_res);
        return true;
    }
    return false;
