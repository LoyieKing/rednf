bool CUserCharacInfo::isSetGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
        if ((m_selected->m_fieldDC3 & flag) == flag)
            return true;
        return false;
    }
    return false;
}