void CUserCharacInfo::SetIsInitSkillFlag(bool flag)
{
    if (m_selected)
    {
        m_selected->m_fieldEB4 = flag;
    }
}