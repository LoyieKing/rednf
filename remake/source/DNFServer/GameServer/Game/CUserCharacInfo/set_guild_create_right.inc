void CUserCharacInfo::set_guild_create_right(unsigned char right)
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    CUR->m_guildCreateRight = right;
    }
}