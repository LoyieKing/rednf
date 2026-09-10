bool CUserCharacInfo::isSetGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
        if ((CUR->m_guildCreateFlag & flag) == flag)
            return true;
        return false;
    }
    return false;
