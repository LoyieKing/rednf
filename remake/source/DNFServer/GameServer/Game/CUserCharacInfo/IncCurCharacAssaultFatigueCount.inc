void CUserCharacInfo::IncCurCharacAssaultFatigueCount()
{
    if (m_selected)
    {
    ++m_selected->m_fieldE6C;
    }
}