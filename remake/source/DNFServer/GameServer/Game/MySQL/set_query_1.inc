bool MySQL::set_query()
{
    if (m_bQueryCounterEnable)
    {
        if (m_pQueryCounter == 0)
        {
            return false;
        }
        std::allocator<char> alloc;
        std::string query(m_query, alloc);
        m_pQueryCounter->IncreaseQueryCount(query);
    }
    int len = strlen(m_query);
    if (len > 0x5fff)
    {
        LogManager::logFormat(1, "DBMgr.cpp", __PRETTY_FUNCTION__, 0x2d1,
                              "[%s][%d]", __PRETTY_FUNCTION__, 0x2d1);
        return false;
    }
    m_queryLen = len;
    return true;
}