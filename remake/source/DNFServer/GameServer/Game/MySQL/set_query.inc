bool MySQL::set_query(const char* fmt, ...)
{
    if (m_bQueryCounterEnable)
    {
        if (m_pQueryCounter == 0)
        {
            return false;
        }
        std::allocator<char> alloc;
        std::string query(fmt, alloc);
        m_pQueryCounter->IncreaseQueryCount(query);
    }
    // [b28d-lane11] ORIG 槽序 ap(-0x28 最深)→query(-0x24)→alloc(-0x1d)；
    // 4.4 对块内局部优先占深槽，故把 ap 段包进独立块使 ap 落深槽。
    {
        va_list ap;
        va_start(ap, fmt);
        vsprintf(m_query, fmt, ap);
        va_end(ap);
    }
    int len = strlen(m_query);
    if (len > 0x5fff)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x169, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x169);
        return false;
    }
    m_queryLen = len;
    return true;
