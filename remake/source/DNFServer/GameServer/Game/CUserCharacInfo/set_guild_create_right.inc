void CUserCharacInfo::set_guild_create_right(unsigned char right)
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    m_selected->m_guildCreateRight = right;
    }
}