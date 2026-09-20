void CUserCharacInfo::resetTempGuildNameAddr()
{
    if (m_selected)
    {
    memset(m_selected->m_padDC5, 0, sizeof(m_selected->m_padDC5));
    memset(m_selected->m_padDDC, 0, sizeof(m_selected->m_padDDC));
    }
}