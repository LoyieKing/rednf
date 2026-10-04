void CUserCharacInfo::ResetCurCharacEventItemCount()
{
    if (m_selected)
    {
    m_selected->m_eventItemCount = 0;
    }
}