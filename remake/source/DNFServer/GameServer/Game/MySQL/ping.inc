int MySQL::ping()
{
    return mysql_ping(m_dbHandle);
