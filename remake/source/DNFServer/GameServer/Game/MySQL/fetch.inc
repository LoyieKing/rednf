bool MySQL::fetch()
{
    if (m_result == 0)
    {
        return false;
    }
    m_row = mysql_fetch_row(m_result);
    if (m_row != 0)
    {
        m_lengths = mysql_fetch_lengths(m_result);
        return true;
    }
    return false;
