void CUserCharacInfo::SetCurCharacLeaveTimeout(bool flag)
{
    if (m_selected)
    {
    m_selected->m_leaveTimeout = flag;
    }
}