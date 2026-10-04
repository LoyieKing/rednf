void CUserCharacInfo::setGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
    m_selected->m_guildCreateFlag = (unsigned short)(m_selected->m_guildCreateFlag | flag);
    }
}