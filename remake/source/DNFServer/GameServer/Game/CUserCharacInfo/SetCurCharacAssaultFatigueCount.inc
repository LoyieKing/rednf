void CUserCharacInfo::SetCurCharacAssaultFatigueCount(int count)
{
    if (m_selected)
    {
    CUR->m_assaultFatigueCount = count;
    }
}