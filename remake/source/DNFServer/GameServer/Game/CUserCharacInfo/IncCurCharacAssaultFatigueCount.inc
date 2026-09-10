void CUserCharacInfo::IncCurCharacAssaultFatigueCount()
{
    if (m_selected)
    {
    ++CUR->m_assaultFatigueCount;
    }
