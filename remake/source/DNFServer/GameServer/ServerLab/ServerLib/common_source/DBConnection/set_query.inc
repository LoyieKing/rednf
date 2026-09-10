bool DBConnection::set_query(const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    vsprintf(m_query, format, vl);
    va_end(vl);
    int len = (int)strlen(m_query);
    if (0x5fff < len)
    {
        return false;
    }
    m_query_len = len;
    return true;
