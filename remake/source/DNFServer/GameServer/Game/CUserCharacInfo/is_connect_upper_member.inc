bool CUserCharacInfo::is_connect_upper_member()
{
    if (m_selected && CUR->m_connectUpperMember != 0)
        return true;
    return false;
