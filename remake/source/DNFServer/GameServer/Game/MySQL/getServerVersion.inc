const char* MySQL::getServerVersion()
{
    return mysql_get_server_info(m_dbHandle);
