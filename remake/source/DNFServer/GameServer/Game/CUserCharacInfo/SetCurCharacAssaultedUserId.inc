void CUserCharacInfo::SetCurCharacAssaultedUserId(unsigned short userId)
{
    if (m_selected)
    {
    CUR->m_assaultedUserId = userId;
    }
