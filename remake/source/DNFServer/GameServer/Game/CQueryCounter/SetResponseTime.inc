void CQueryCounter::SetResponseTime(double responseTime)
{
    if (m_current != m_queryInfoMap.end())
    {
        m_current->second.responseTime += responseTime;
        m_current = m_queryInfoMap.end();
    }
