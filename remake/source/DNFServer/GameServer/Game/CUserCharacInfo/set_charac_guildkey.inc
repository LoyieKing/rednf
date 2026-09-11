void CUserCharacInfo::set_charac_guildkey(unsigned int key)
{
    if (m_selected)
    {
    CUR->m_guildkey = key;
    }
}