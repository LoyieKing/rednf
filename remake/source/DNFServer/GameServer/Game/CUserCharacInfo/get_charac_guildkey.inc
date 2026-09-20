int CUserCharacInfo::get_charac_guildkey()
{
    return m_selected ? m_selected->m_guildId : 0;
}