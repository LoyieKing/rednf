bool CUserCharacInfo::is_connect_upper_member()
{
    if (m_selected && m_selected->m_fieldE26 != 0)
        return true;
    return false;
}