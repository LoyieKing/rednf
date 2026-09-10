unsigned long MySQL::escape_string(char* dst, const char* src)
{
    return mysql_real_escape_string(m_dbHandle, dst, src, strlen(src));
