void CUserCharacInfo::setGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
    m_selected->m_fieldDC3 = (unsigned short)(m_selected->m_fieldDC3 | flag);
    }
}