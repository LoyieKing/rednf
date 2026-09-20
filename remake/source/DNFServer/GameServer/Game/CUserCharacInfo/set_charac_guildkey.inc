void CUserCharacInfo::set_charac_guildkey(unsigned int key)
{
    if (m_selected)
    {
    m_selected->m_guildId = key;
    }
}