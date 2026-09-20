void CUserCharacInfo::SetIsInitSkillFlag2ND(bool flag)
{
    if (m_selected)
    {
        m_selected->m_padEB6 = flag;
    }
}