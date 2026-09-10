void CUserCharacInfo::SetCurCharacLeaveTimeout(bool flag)
{
    if (m_selected)
    {
    CUR->m_leaveTimeout = flag;
    }
