void CUserCharacInfo::setGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
    CUR->m_guildCreateFlag = (unsigned short)(CUR->m_guildCreateFlag | flag);
    }
}