void CUserCharacInfo::SetCurCharacAssaultFatigueCount(int count)
{
    if (m_selected)
    {
    m_selected->m_fieldE6C = count;
    }
}