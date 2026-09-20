void CUserCharacInfo::SetCurCharacSuperState(bool flag)
{
    if (m_selected)
    {
    m_selected->m_fieldE65 = flag;
    }
}