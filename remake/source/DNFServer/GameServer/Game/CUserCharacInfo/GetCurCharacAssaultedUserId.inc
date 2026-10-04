unsigned short CUserCharacInfo::GetCurCharacAssaultedUserId()
{
    return m_selected ? m_selected->m_assaultedUserId : 0;
}