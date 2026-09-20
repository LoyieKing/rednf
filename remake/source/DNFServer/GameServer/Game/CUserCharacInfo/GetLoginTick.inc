long CUserCharacInfo::GetLoginTick()
{
    if (!m_selected)
        return 0;
    return m_selected->m_fieldE02;
}