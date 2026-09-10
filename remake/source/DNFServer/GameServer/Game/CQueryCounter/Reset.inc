void CQueryCounter::Reset()
{
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); ++it)
    {
        it->second.count = 0;
        it->second.responseTime = 0.0;
    }
    m_totalQueryCount = 0;
