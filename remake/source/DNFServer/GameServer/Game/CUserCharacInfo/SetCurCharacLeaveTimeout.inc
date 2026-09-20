void CUserCharacInfo::SetCurCharacLeaveTimeout(bool flag)
{
    if (m_selected)
    {
    m_selected->m_fieldE67 = flag;
    }
}