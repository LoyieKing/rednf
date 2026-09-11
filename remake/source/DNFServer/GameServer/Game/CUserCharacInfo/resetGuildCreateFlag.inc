void CUserCharacInfo::resetGuildCreateFlag()
{
    if (m_selected)
    {
    CUR->m_guildCreateFlag = 0;
    }
}