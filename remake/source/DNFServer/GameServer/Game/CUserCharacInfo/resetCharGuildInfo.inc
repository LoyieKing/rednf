void CUserCharacInfo::resetCharGuildInfo()
{
    if (m_selected)
    {
    m_selected->m_guildId = 0;
    m_selected->m_fieldDED = 0;
    m_selected->m_fieldDE9 = 0;
    m_selected->m_fieldDF3 = 0;
    m_selected->m_fielddc2 = 0;
    m_selected->m_fieldDC3 = 0;
    m_selected->m_fieldDEF = 0;
    }
}