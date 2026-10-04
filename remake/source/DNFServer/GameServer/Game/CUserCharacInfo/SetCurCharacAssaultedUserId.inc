void CUserCharacInfo::SetCurCharacAssaultedUserId(unsigned short userId)
{
    if (m_selected)
    {
    m_selected->m_assaultedUserId = userId;
    }
}