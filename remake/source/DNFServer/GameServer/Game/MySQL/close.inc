void MySQL::close()
{
    if (m_dbHandle != 0)
    {
        mysql_close(m_dbHandle);
        m_dbHandle = 0;
    }
