void CUserCharacInfo::SetIsInitSkillFlag(bool flag)
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag = flag;
    }
}