bool CEventStayTime::IsFirstReset()
{
    if (m_firstReset) {
        m_firstReset = false;
        return true;
    }
    return m_firstReset;
