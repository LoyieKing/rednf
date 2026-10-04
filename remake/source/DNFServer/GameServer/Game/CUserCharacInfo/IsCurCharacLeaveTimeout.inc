bool CUserCharacInfo::IsCurCharacLeaveTimeout()
{
    return m_selected ? m_selected->m_leaveTimeout : false;
}