void CUserCharacInfo::IncCurCharacAssaultFatigueCount()
{
    if (m_selected)
    {
    ++m_selected->m_assaultFatigueCount;
    }
}