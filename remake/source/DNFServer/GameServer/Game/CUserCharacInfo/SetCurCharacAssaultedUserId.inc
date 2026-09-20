void CUserCharacInfo::SetCurCharacAssaultedUserId(unsigned short userId)
{
    if (m_selected)
    {
    m_selected->m_field12B5 = userId;
    }
}