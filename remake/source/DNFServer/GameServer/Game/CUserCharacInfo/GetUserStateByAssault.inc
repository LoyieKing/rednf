int CUserCharacInfo::GetUserStateByAssault()
{
    if (m_selected != 0)
    {
        if (m_selected->m_fieldE65 != 0)
            return 6;
        if (m_selected->m_fieldE66 != 0)
            return 4;
        if (m_selected->m_fieldE41 != 0)
            return 5;
    }
    return 0;
}