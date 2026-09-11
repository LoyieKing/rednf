long CUserCharacInfo::GetLoginTick()
{
    if (!m_selected)
        return 0;
    return CUR->m_loginTick;
}