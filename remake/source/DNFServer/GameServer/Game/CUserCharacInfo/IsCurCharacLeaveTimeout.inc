bool CUserCharacInfo::IsCurCharacLeaveTimeout()
{
    return m_selected ? CUR->m_leaveTimeout : false;
}