void CUserCharacInfo::ResetCurCharacEventAddItemCount()
{
    if (m_selected)
    {
    m_selected->m_eventAddItemCount = 0;
    }
}