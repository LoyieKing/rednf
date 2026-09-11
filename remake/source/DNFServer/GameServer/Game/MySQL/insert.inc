int MySQL::insert(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(m_query, fmt, ap);
    va_end(ap);
    if (!set_query())
    {
        return 3;
    }
    if (!exec(true))
    {
        return 5;
    }
    return 0;
}