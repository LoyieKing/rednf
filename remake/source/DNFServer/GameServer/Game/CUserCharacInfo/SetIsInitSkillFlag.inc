void CUserCharacInfo::SetIsInitSkillFlag(bool flag)
{
    if (m_selected)
    {
        m_selected->m_isInitSkillFlag = flag;
    }
}