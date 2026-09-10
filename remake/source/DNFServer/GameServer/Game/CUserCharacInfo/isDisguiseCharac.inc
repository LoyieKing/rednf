bool CUserCharacInfo::isDisguiseCharac()
{
    if (m_selected)
    {
        return CUR->m_isDisguiseCharac;
    }
    return false;
