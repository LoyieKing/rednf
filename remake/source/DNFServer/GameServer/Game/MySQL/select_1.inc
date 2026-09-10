int MySQL::select(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(m_query, fmt, ap);
    va_end(ap);
    return select();
