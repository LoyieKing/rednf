void CUserCharacInfo::resetGuildCreateFlag()
{
    if (m_selected)
    {
    m_selected->m_guildCreateFlag = 0;
    }
}